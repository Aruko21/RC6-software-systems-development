#ifndef LAB_DEVSSYS_BUFFERS_H
#define LAB_DEVSSYS_BUFFERS_H

#include <sys/types.h>

#include "general.h"

typedef struct buffer_t {
    size_t curr_size;        // total size (in bytes)
    char *data;
} buffer_t;

typedef struct document_links {
    size_t curr_size;
    size_t links_number;
    char (*links)[BUF_SIZE];
} document_links;

buffer_t *allocate_buffer(size_t initial_size);
void free_buffer(buffer_t *buf);
int reallocate_buffer(buffer_t *buf);

document_links *allocate_links(size_t initial_amount);
void free_links(document_links *buf);
int reallocate_links(document_links *buf);

#endif //LAB_DEVSSYS_BUFFERS_H
