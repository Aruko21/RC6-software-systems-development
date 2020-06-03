#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "general.h"
#include "buffers.h"
#include "parsers.h"

#include "http_client.h"

HttpRequest create_GET_req(char *url_domain, char *url_path) {
    HttpRequest request;
    request.method = "GET";
    request.path = url_path;
    request.version = "1.1";
    request.host = url_domain;
    request.user_agent = "Simple HTTP Client on C";
    request.connection = "Close";

    return request;
}

int connect_to_srv(const char *url_domain) {
    struct addrinfo hints;
    struct addrinfo *res;
    int status = 0;

    memset((char *) &hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Заполняет двусвязный список в res, с информацией о нужном соединении. Список, т.к. для одного
    // доменного имени может быть несколько адресов?
    status = getaddrinfo(url_domain, "80", &hints, &res);
    if (status != 0) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Error while getting addrinfo: %s", gai_strerror(status));
        print_error(msg_buf, __func__);

        return -1;
    }

    int sid = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // socket id
    if (sid == -1) {
        perror("Error while creating socket");

        return -1;
    }


    status = connect(sid, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
        perror("Error while connecting to server");
        close(sid);

        return -1;
    }

    freeaddrinfo(res);
    return sid;
}

int send_to_server(HttpRequest request, int socket) {
    char send_buf[SEND_BUF];

    sprintf(send_buf, "%s %s HTTP/%s\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: %s\r\n\r\n",
            request.method, request.path, request.version, request.host, request.user_agent, request.connection);

    size_t total_bytes = 0;
    size_t message_len = strlen(send_buf);
    while (total_bytes < message_len) {
        int bytes = send(socket, send_buf + total_bytes, strlen(send_buf + total_bytes), 0);
        if (bytes == -1) {
            perror("Error while sending");
            return -1;
        }
        total_bytes += bytes;
    }
    return 0;
}

int receive_from_server(buffer_t *html_page, int socket_id) {
    char *html_ptr = (char *) html_page->data;
    int response_length = 0;
    while (1) {
        char recv_buf[1024];
        int bytes = recv(socket_id, recv_buf, 1024, 0);

        if (bytes == 0) {
            break;
        }

        if (bytes == -1) {
            perror("Error while receive");
            return -1;
        }

        while (response_length + bytes > html_page->curr_size) {
            size_t offset = html_ptr - (char *) html_page->data;
            int status = reallocate_buffer(html_page);
            if (status < 0) {
                perror("Error while reallocation");
                return -1;
            }

            html_ptr = (char *) html_page->data + offset;
        }

        memcpy(html_ptr, recv_buf, bytes);

        response_length += bytes;
        html_ptr += bytes;
    }

    *html_ptr = '\0';

    return response_length;
}