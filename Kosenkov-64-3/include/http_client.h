#ifndef LAB_DEVSSYS_HTTP_CLIENT_H
#define LAB_DEVSSYS_HTTP_CLIENT_H

#include "buffers.h"

#define HREF_FLAG 1

typedef struct HttpRequest {
    char *method;
    char *path;
    char *version;
    char *host;
    char *user_agent;
    char *connection;
} HttpRequest;

int connect_to_srv(const char *url_domain);
int send_to_server(HttpRequest request, int socket);
int receive_from_server(buffer_t *html_page, int socket_id);
HttpRequest create_GET_req(char *url_domain, char *url_path);

#endif //LAB_DEVSSYS_HTTP_CLIENT_H
