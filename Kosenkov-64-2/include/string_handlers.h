#ifndef LAB_DEVSSYS_STRING_HANDLERS_H
#define LAB_DEVSSYS_STRING_HANDLERS_H

typedef void (*string_handler)(const char*, char**);

void str_translation(const char *input, char **output);
void str_inverting(const char *input, char **output);
void str_exchange(const char *input, char **output);
void str_to_KOI8(const char *input, char **output);

void str_to_uppercase(const char *input, char **output);
void str_to_lowercase(const char *input, char **output);
void str_invert_case(const char *input, char **output);

#endif //LAB_DEVSSYS_STRING_HANDLERS_H
