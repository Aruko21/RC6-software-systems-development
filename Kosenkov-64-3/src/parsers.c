#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

#include "parsers.h"
#include "general.h"
#include "buffers.h"

int domain_path_parser(const char *url, char **domain, char **path, int href_flag) {
    regex_t regex;
    int reg_status = 0;

    if (!href_flag) {
        reg_status = regcomp(&regex, "([-A-Za-z0-9.]+)(/.*)", REG_EXTENDED);
    } else {
        reg_status = regcomp(&regex, "((http|https)://([-A-Za-z0-9.]+))?(/?.*)", REG_EXTENDED);
    }

    if (reg_status != 0) {
        perror("Couldn't compile regex");

        return -1;
    }

    size_t n = 8; // number of matches
    regmatch_t *pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * n);

    reg_status = regexec(&regex, url, n, pmatch, 0);
    if (reg_status == REG_NOMATCH) {
        print_error("Regular expression has no match", __func__);

        free(pmatch);
        return -1;
    }

//    for (size_t i = 0; pmatch[i].rm_so != -1 && i < n; ++i) {
//        printf("For i=%lu: '%.*s'\n", i, pmatch[i].rm_eo - pmatch[i].rm_so, url + pmatch[i].rm_so);
//    }

    if (!href_flag) { // Parse argv argument (without http://)
        // pmatch[0] = whole expression
        size_t domain_len = pmatch[1].rm_eo - pmatch[1].rm_so;
        memcpy(*domain, url + pmatch[1].rm_so, domain_len);
        (*domain)[domain_len] = '\0';

        // If there are '/...' after domain
        if (pmatch[2].rm_so != -1) {
            size_t path_len = pmatch[2].rm_eo - pmatch[2].rm_so;
            memcpy(*path, url + pmatch[2].rm_so, path_len);
            (*path)[path_len] = '\0';

        } else { // If not, hardcode root
            strcpy(*path, "/");
        }

    } else { // Parse href links (with or without http://)
        if (pmatch[1].rm_so == -1) { // If there are relative url (without http://), then whole link is the path
            (*domain)[0] = '\0';
            size_t path_len = pmatch[0].rm_eo - pmatch[0].rm_so;
            memcpy(*path, url + pmatch[0].rm_so, path_len);
            (*path)[path_len] = '\0';

        } else { // If not, then there are an absolute url
            size_t domain_length = pmatch[3].rm_eo - pmatch[3].rm_so;
            memcpy(*domain, url + pmatch[3].rm_so, domain_length);
            (*domain)[domain_length] = '\0';

            // If there are '/...' after domain
            if (pmatch[4].rm_eo - pmatch[4].rm_so != 0) {
                size_t path_len = pmatch[4].rm_eo - pmatch[4].rm_so;
                memcpy(*path, url + pmatch[4].rm_so, path_len);
                (*path)[path_len] = '\0';

            } else { // If not, hardcode root
                strcpy(*path, "/");
            }
        }
    }

    free(pmatch);
    regfree(&regex);

    return 0;
}


int parse_links(const char *html_doc, size_t content_length, document_links*links) {
    regex_t regex;
    int reg_status = 0;

    reg_status = regcomp(&regex, "<body.*>", REG_ICASE);
    if (reg_status != 0) {
        perror("Couldn't compile regex");

        return -1;
    }

    size_t n = 1; // number of matches
    regmatch_t *pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * n);

    reg_status = regexec(&regex, html_doc, n, pmatch, 0);
    if (reg_status == REG_NOMATCH) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Response has no <body>\n");
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        links->links[0][0] = '\0';
        links->links_number = 0;
        free(pmatch);
        return 0;
    }

    size_t body_offset = pmatch[0].rm_so;

    const char *links_ptr = html_doc + body_offset;

    regfree(&regex);
    reg_status = regcomp(&regex, "href=[\"']([^\"']+)[\"']", REG_EXTENDED | REG_ICASE);
    if (reg_status != 0) {
        perror("Couldn't compile regex");

        return -1;
    }

    n = 2;
    free(pmatch);
    pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * n);

    size_t links_num = 0;
    while (regexec(&regex, links_ptr, n, pmatch, 0) != REG_NOMATCH) {
        while (links->curr_size < links_num + 1) {
            int status = reallocate_links(links);
            if (status < 0) {
                perror("Error while reallocation");

                free(pmatch);
                regfree(&regex);
                return -1;
            }
        }

        size_t link_length = pmatch[1].rm_eo - pmatch[1].rm_so;
        memcpy(links->links[links_num], links_ptr + pmatch[1].rm_so, link_length);
        links->links[links_num][link_length] = '\0';

        ++links_num;
        links_ptr += pmatch[0].rm_eo;
    }
    links->links_number = links_num;
    links->links[links_num][0] = '\0';

    free(pmatch);
    regfree(&regex);

    return 0;
}