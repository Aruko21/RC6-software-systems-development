#ifndef LAB_DEVSSYS_SECOND_THREAD_H
#define LAB_DEVSSYS_SECOND_THREAD_H

#include "string_handlers.h"

string_handler get_second_thread_handler();

void *second_handler(void *arg_p);

#endif //LAB_DEVSSYS_SECOND_THREAD_H
