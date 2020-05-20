#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "second_thread.h"
#include "general.h"

void *second_handler(void *arg_p) {
    char *message = "Second thread start\n";
    write(STDOUT_FILENO, message, strlen(message));

    while (1) {
        pthread_mutex_lock(&mutx2);

        while (intermediate_str_len == 0 && done == 0) {
            pthread_cond_wait(&cond_can_second_handle, &mutx2);
        }

        if (done) {
            pthread_mutex_unlock(&mutx2);
            pthread_exit(0);
        }


        char *tmp_message = "Second thread string: \n";
        write(STDOUT_FILENO, tmp_message, strlen(tmp_message));
        write(STDOUT_FILENO, intermediate_str_buf, intermediate_str_len);
        intermediate_str_len = 0;
        memset(input_str_buf, '\0', BUF_SIZE);

        pthread_cond_signal(&cond_can_first_send);
        pthread_mutex_unlock(&mutx2);
    }
}