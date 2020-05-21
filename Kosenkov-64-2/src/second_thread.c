#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "second_thread.h"
#include "general.h"
#include "string_handlers.h"

string_handler get_second_thread_handler() {
    string_handler handler_func = NULL;

    switch(modes[1]) {
        case 1: {
            handler_func = str_translation;
            break;
        }
        case 2: {
            handler_func = str_to_uppercase;
            break;
        }
        case 3: {
            handler_func = str_to_lowercase;
            break;
        }
        case 4: {
            handler_func = str_invert_case;
            break;
        }
        default: {
            handler_func = NULL;
        }
    }

    return handler_func;
}

void *second_handler(void *arg_p) {
    char *tmp_buf = (char *)malloc(BUF_SIZE);
    char *formatted_string = (char *)malloc(BUF_SIZE);
    int length = 0;

    char *message = "Second thread start\n";
    write(STDOUT_FILENO, message, strlen(message));

    while (1) {
        pthread_mutex_lock(&mutx2);

        while (intermediate_str_len == 0 && done == 0) {
            pthread_cond_wait(&cond_can_second_handle, &mutx2);
        }

        if (done) {
            pthread_mutex_unlock(&mutx2);
            break;
        }

        strncpy(tmp_buf, intermediate_str_buf, intermediate_str_len + 1);
        memset(intermediate_str_buf, '\0', BUF_SIZE);
        length = intermediate_str_len;
        intermediate_str_len = 0;

        pthread_cond_signal(&cond_can_first_send);
        pthread_mutex_unlock(&mutx2);

        string_handler handler_func = get_second_thread_handler();
        handler_func(tmp_buf, &formatted_string);

        char *tmp_message = "Second thread string: \n";
        write(STDOUT_FILENO, tmp_message, strlen(tmp_message));
        write(STDOUT_FILENO, formatted_string, length);
        write(STDOUT_FILENO, "\n", 1);
    }

    free(tmp_buf);
    free(formatted_string);
    pthread_exit(0);
}