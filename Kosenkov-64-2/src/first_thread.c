#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "first_thread.h"
#include "general.h"

void *first_handler(void *arg_p) {
    char tmp_buf[BUF_SIZE];
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
            pthread_exit(0);
        }

        strncpy(tmp_buf, input_str_buf, input_str_len);
        memset(input_str_buf, '\0', BUF_SIZE);
        length = input_str_len;
        input_str_len = 0;

        pthread_cond_signal(&cond_can_input);
        pthread_mutex_unlock(&mutx1);

//        for (int i = 0; i < (length / 2); ++i) {
//            char sym = tmp_buf[i];
//            tmp_buf[i] = tmp_buf[length - 2 - i];
//            tmp_buf[length - 2 - i] = sym;
//        };

        char *tmp_message = "First thread string: \n";
        write(STDOUT_FILENO, tmp_message, strlen(tmp_message));
        write(STDOUT_FILENO, tmp_buf, length);

        intermediate_str_len = length;

        strncpy(intermediate_str_buf, tmp_buf, intermediate_str_len);

        pthread_cond_signal(&cond_can_second_handle);

        while (intermediate_str_len != 0) {
            pthread_cond_wait(&cond_can_first_send, &mutx2);
        }
    }
}