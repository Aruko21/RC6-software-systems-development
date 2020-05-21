#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "first_thread.h"
#include "general.h"
#include "string_handlers.h"

string_handler get_first_thread_handler() {
    string_handler handler_func = NULL;

    switch(modes[0]) {
        case 1: {
            handler_func = str_translation;
            break;
        }
        case 2: {
            handler_func = str_inverting;
            break;
        }
        case 3: {
            handler_func = str_exchange;
            break;
        }
        case 4: {
            handler_func = str_to_KOI8;
            break;
        }
        default: {
            handler_func = NULL;
        }
    }

    return handler_func;
}

void *first_handler(void *arg_p) {
    char *tmp_buf = (char *)malloc(BUF_SIZE);
    char *formatted_string = (char *)malloc(BUF_SIZE);
    int length = 0;

    char *message = "First thread start\n";
    write(STDOUT_FILENO, message, strlen(message));

    pthread_mutex_lock(&mutx2);

    while (1) {
        pthread_mutex_lock(&mutx1);

        while (input_str_len == 0 && done == 0) {
            pthread_cond_wait(&cond_can_first_handle, &mutx1);
        }

        if (done) {
            pthread_cond_signal(&cond_can_second_handle);
            pthread_mutex_unlock(&mutx1);
            pthread_mutex_unlock(&mutx2);
            break;
        }

        strncpy(tmp_buf, input_str_buf, input_str_len + 1);
        memset(input_str_buf, '\0', BUF_SIZE);
        length = input_str_len;
        input_str_len = 0;

        pthread_cond_signal(&cond_can_input);
        pthread_mutex_unlock(&mutx1);

        string_handler handler_func = get_first_thread_handler();
        handler_func(tmp_buf, &formatted_string);

        char *tmp_message = "First thread string: \n";
        write(STDOUT_FILENO, tmp_message, strlen(tmp_message));
        write(STDOUT_FILENO, formatted_string, length);
        write(STDOUT_FILENO, "\n", 1);

        intermediate_str_len = length;

        strncpy(intermediate_str_buf, formatted_string, intermediate_str_len + 1);

        pthread_cond_signal(&cond_can_second_handle);

        while (intermediate_str_len != 0) {
            pthread_cond_wait(&cond_can_first_send, &mutx2);
        }
    }

    free(tmp_buf);
    free(formatted_string);
    pthread_exit(0);
}