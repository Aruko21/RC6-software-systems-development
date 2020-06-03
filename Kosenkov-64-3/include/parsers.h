#ifndef LAB_DEVSSYS_PARSERS_H
#define LAB_DEVSSYS_PARSERS_H

#include "buffers.h"

int domain_path_parser(const char *url, char **domain, char **path, int href_flag);
int parse_links(const char *html_doc, size_t content_length, document_links *links);

#endif //LAB_DEVSSYS_PARSERS_H
