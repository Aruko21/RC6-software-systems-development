// TODO: Почему не могу перезапускать без ошибки bind?


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>

#define HTTP_PORT 80

#define BUF_SIZE 256
#define URL_SIZE 512

struct HttpRequestHdrs {
    char *method;
    char *path;
    char *version;
    char *host;
    char *user_agent;
};

int domain_path_parser(const char *url, char **domain, char **path) {
    regex_t regex;
    int reg_status = 0;

    reg_status = regcomp(&regex, "([-A-Za-z0-9.]+)(/(.*))?", REG_EXTENDED);
    if (reg_status != 0) {
        perror("Couldn't compile regex");

        return -1;
    }

    size_t n = 3; // number of matches
    regmatch_t *pmatch = (regmatch_t *) malloc(sizeof(regmatch_t) * n);

    reg_status = regexec(&regex, url, 3, pmatch, 0);
    if (reg_status == REG_NOMATCH) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Regular expression has no match\n");
        write(STDERR_FILENO, msg_buf, strlen(msg_buf));

        free(pmatch);

        return -1;
    }

    // pmatch[0] = whole expression
    size_t domain_len = pmatch[1].rm_eo - pmatch[1].rm_so;
    memcpy(*domain, url + pmatch[1].rm_so, domain_len);
    (*domain)[domain_len] = '\0';


    if (pmatch[2].rm_so != -1) {
        size_t path_len = pmatch[2].rm_eo - pmatch[2].rm_so;
        memcpy(*path, url + pmatch[2].rm_so, domain_len);
        (*path)[path_len] = '\0';
    } else {
        *path = "/";
    }

    free(pmatch);
    regfree(&regex);

    return 0;
}


int main(int argc, const char **argv) {
    if (argc < 2) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Missing URL address. Usage: %s, <URL>\n", argv[0]);
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        return -1;
    }

    char srv_url[URL_SIZE];
    strcpy(srv_url, argv[1]);

    int status = 0;

    char *url_domain = (char *) malloc(sizeof(char) * BUF_SIZE);
    char *url_path = (char *) malloc(sizeof(char) * BUF_SIZE);

    status = domain_path_parser(srv_url, &url_domain, &url_path);
    if (status != 0) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Error while parsing url\n");
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        free(url_domain);
        free(url_path);
        return -1;
    }

    printf("Check domain: '%s' and path: '%s'\n", url_domain, url_path);


    struct addrinfo hints;
    struct addrinfo *res;


    memset((char *) &hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Заполняет двусвязный список в res, с информацией о нужном соединении. Список, т.к. для одного
    // доменного имени может быть несколько адресов?
    status = getaddrinfo(url_domain, "80", &hints, &res);
    if (status != 0) {
        char msg_buf[BUF_SIZE];
        sprintf(msg_buf, "Error while getting addrinfo: %s\n", gai_strerror(status));
        write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

        free(url_domain);
        free(url_path);
        return -1;
    }

    int sid = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // socket id
    if (sid == -1) {
        perror("Error while creating socket");

        free(url_domain);
        free(url_path);
        return -1;
    }


    status = connect(sid, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
        perror("Error while connecting to server");
        close(sid);

        free(url_domain);
        free(url_path);
        return -1;
    }

    // TODO: соединение есть. Сделать отправку запроса, и получение ответа
    // TODO: проработать стандартный вариант подключения по той статье, где описан getaddrinfo
    struct HttpRequestHdrs request;
    request.method = "GET";
    request.path = url_path;
    request.version = "1.1";
    request.host = url_domain;
    request.user_agent = "Simple HTTP Client on C";


    freeaddrinfo(res);
    close(sid);

    return 0;
}