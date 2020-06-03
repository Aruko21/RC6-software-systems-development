#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "general.h"
#include "buffers.h"
#include "parsers.h"
#include "http_client.h"


void print_error(const char *err_msg, const char *func_name) {
    char msg_buf[BUF_SIZE];
    sprintf(msg_buf, "%s: %s\n", func_name, err_msg);
    write(STDERR_FILENO, msg_buf, strlen(msg_buf));
}

int main(int argc, const char **argv) {
    if (argc < 2) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Missing URL address. Usage: %s <URL>", argv[0]);
        print_error(msg_buf, __func__);

        return -1;
    }

    int is_done = 0;
    int is_error = 0;

    char srv_url[URL_SIZE];
    strcpy(srv_url, argv[1]);

    int status = 0;

    char *url_domain = (char *) malloc(sizeof(char) * BUF_SIZE);
    char *url_path = (char *) malloc(sizeof(char) * BUF_SIZE);

    status = domain_path_parser(srv_url, &url_domain, &url_path, 0);
    if (status != 0) {
        print_error("Error while parsing url", __func__);

        free(url_domain);
        free(url_path);
        return -1;
    }

    int sid = connect_to_srv(url_domain);
    if (sid < 0) {
        print_error("Error while connecting to server", __func__);
        free(url_domain);
        free(url_path);

        return -1;
    }

    buffer_t *html_page = allocate_buffer(MIN_DYN_BUF);
    if (!html_page) {
        perror("Error while allocating memory for html page buffer");
        free(url_domain);
        free(url_path);
        close(sid);

        return -1;
    }

    document_links *links = allocate_links(MIN_LINKS);

    do {
        printf("\nCrossing to domain: '%s' with path: '%s'\n", url_domain, url_path);

        HttpRequest request = create_GET_req(url_domain, url_path);

        status = send_to_server(request, sid);
        if (status < 0) {
            print_error("Error while sending request", __func__);
            is_error = 1;
            break;
        }

        int response_length = receive_from_server(html_page, sid);

        if (response_length < 0) {
            print_error("Error while receiving response", __func__);
            is_error = 1;
            break;
        }

        write(STDOUT_FILENO, html_page->data, response_length);

        parse_links(html_page->data, response_length, links);

        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "\n-----END OF HTML-----\n\nChoose number of link from below to cross:\n");
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
        sprintf(msg_buf, "0 - Exit from HTTP-Client Application\n");
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        if (links->links_number == 0) {
            sprintf(msg_buf, "- HTML page has no links -\n");
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
        }

        for (size_t i = 0; i < links->links_number; ++i) {
            sprintf(msg_buf, "%lu - '%s'\n", i + 1, links->links[i]);
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
        }

        sprintf(msg_buf, "\nPlease, input link number you want cross to:\n");
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        char input_buf[BUF_SIZE];
        int menu_item = -1;
        while (menu_item < 0) {
            int menu_input = read(0, input_buf, BUF_SIZE);
            char *endptr;
            int input_to_digit = strtol(input_buf, &endptr, 10);

            if (input_buf == endptr) {
                sprintf(msg_buf, "Your input is incorrect, please, enter only number\n");
                write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
                continue;
            }

            if (input_to_digit > links->links_number || input_to_digit < 0) {
                sprintf(msg_buf, "Your number must be in range (0, %lu)\n", links->links_number);
                write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
                continue;
            }
            menu_item = input_to_digit;
        }

        if (menu_item == 0) {
            sprintf(msg_buf, "Exiting application...\n");
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
            is_done = 1;
        } else {
            char *new_domain = (char *) malloc(sizeof(char) * BUF_SIZE);
            status = domain_path_parser(links->links[menu_item - 1], &new_domain, &url_path, HREF_FLAG);
            if (status != 0) {
                print_error("Error while parsing url", __func__);
                is_error = 1;
                break;
            }

            if (new_domain[0] == '\0' || strcmp(new_domain, url_domain) == 0) {
                free(new_domain);
            } else {
                free(url_domain);
                url_domain = new_domain;
            }

            close(sid);
            sprintf(msg_buf, "Reconnect to '%s' domain...\n", url_domain);
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
            sid = connect_to_srv(url_domain);
        }

    } while (!is_done);


    free_links(links);
    free_buffer(html_page);
    free(url_domain);
    free(url_path);
    close(sid);

    if (is_error) {
        return -1;
    }

    return 0;
}