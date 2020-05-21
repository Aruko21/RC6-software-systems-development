#include <string.h>
#include <ctype.h>

#include "string_handlers.h"

void str_translation(const char *input, char **output) {
    strncpy(*output, input, strlen(input));
}

void str_inverting(const char *input, char **output) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; ++i) {
        (*output)[length - 1 - i] = input[i];
    };

    (*output)[length] = '\0';
}

void str_exchange(const char *input, char **output) {
    size_t length = strlen(input);
    size_t even_length = length - (length % 2);
    for (size_t i = 0; i < even_length ; i += 2) {
        (*output)[i] = input[i + 1];
        (*output)[i + 1] = input[i];
    };

    if (length > even_length) {
        (*output)[length - 1] = input[length - 1];
    }

    (*output)[length] = '\0';
}

void str_to_KOI8(const char *input, char **output) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; ++i) {
        if (islower(input[i])) {
            (*output)[i] = input[i] - 32;
        } else if (isupper(input[i])) {
            (*output)[i] = input[i] + 32;
        }

        (*output)[i] |= (1 << 7);
    }

    (*output)[length] = '\0';
}

void str_to_uppercase(const char *input, char **output) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; ++i) {
        (*output)[i] = toupper(input[i]);
    };

    (*output)[length] = '\0';
}

void str_to_lowercase(const char *input, char **output) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; ++i) {
        (*output)[i] = tolower(input[i]);
    };

    (*output)[length] = '\0';
}

void str_invert_case(const char *input, char **output) {
    size_t length = strlen(input);

    for (size_t i = 0; i < length; ++i) {
        if (islower(input[i])) {
            (*output)[i] = toupper(input[i]);
        } else if (isupper(input[i])) {
            (*output)[i] = tolower(input[i]);
        }
    };

    (*output)[length] = '\0';
}
