#ifndef LAB_DEVSSYS_PARSERS_H
#define LAB_DEVSSYS_PARSERS_H

#include "buffers.h"

enum parser_returns {
    PARSE_ERROR = -1,
    SUCCESS = 0,
    ABSOLUTE_PATH,
    RELATIVE_PATH
};

int domain_path_parser(const char *url, char **domain, char **path, int href_flag);
int parse_links(const char *html_doc, size_t content_length, document_links *links);

#endif //LAB_DEVSSYS_PARSERS_H
