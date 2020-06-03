#include <sys/types.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "buffers.h"


buffer_t *allocate_buffer(size_t initial_size) {
    buffer_t *buf = (buffer_t *) malloc(sizeof(buffer_t));
    if (!buf) {
        return NULL;
    }

    buf->data = malloc(sizeof(char) * initial_size);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->curr_size = initial_size;
    return buf;
}

document_links *allocate_links(size_t initial_amount) {
    document_links *buf = (document_links *) malloc(sizeof(document_links));
    if (!buf) {
        return NULL;
    }

    buf->links = malloc(sizeof(char[BUF_SIZE]) * initial_amount);
    if (!buf->links) {
        free(buf);
        return NULL;
    }

    buf->curr_size = initial_amount;
    return buf;
}

void free_buffer(buffer_t *buf) {
    if (buf) {
        free(buf->data);
        free(buf);
    }
}

void free_links(document_links *buf) {
    if (buf) {
        free(buf->links);
        free(buf);
    }
}

int reallocate_links(document_links *buf) {
    if (!buf) {
        return -1;
    }

    char (*new_buf)[BUF_SIZE] = realloc(buf->links, (size_t)(buf->curr_size * sizeof(char[BUF_SIZE]) * REALLOC_COEF));
    if (!new_buf) {
        return -1;
    }
    buf->links = new_buf;
    buf->curr_size *= REALLOC_COEF;

    return 0;
}

int reallocate_buffer(buffer_t *buf) {
    if (!buf) {
        return -1;
    }
    void *new_buf = realloc(buf->data, (size_t) (buf->curr_size * REALLOC_COEF));
    if (!new_buf) {
        return -1;
    }

    buf->data = new_buf;
    buf->curr_size *= REALLOC_COEF;

    return 0;
}
