#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "main_thread.h"
#include "general.h"

char command_flag = 0;

void command_handler() {
    command_flag = 1;
}

void command_reader() {
    char *message = "\nSwitching threads modes.\n";
    write(STDOUT_FILENO, message, strlen(message));
    message = "Enter 2 digits that indicate thread 1 and 2 modes. '0' for keep mode unchanged\n";
    write(STDOUT_FILENO, message, strlen(message));
    message = "Example: 23 (will set 1st thread in mode 2 and 2nd thread in mode 3)\n";
    write(STDOUT_FILENO, message, strlen(message));

    int is_command_correct = 0;

    while (!is_command_correct) {
        int commands_str_len = read(0, input_str_buf, BUF_SIZE);

        if (commands_str_len == 3) {
            if ((input_str_buf[0] >= '0' && input_str_buf[0] <= '4') &&
                (input_str_buf[1] >= '0' && input_str_buf[1] <= '4')) {
                is_command_correct = 1;
            }
        }

        if (!is_command_correct) {
            message = "Incorrect command. Please, try again\n";
            write(STDOUT_FILENO, message, strlen(message));
        }
    }

    modes[0] = input_str_buf[0] == '0' ? modes[0] : input_str_buf[0] - '0';
    modes[1] = input_str_buf[1] == '0' ? modes[1] : input_str_buf[1] - '0';

    char msg_buf[64];
    sprintf(msg_buf, "Current modes: Thread1 - %d, Thread2 - %d\n", modes[0], modes[1]);
    write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
}

int str_normalize_enter(char *string) {
    size_t length = strlen(string);

    if (string[length - 1] == '\n') {
        string[length - 1] = '\0';
        return 1;
    }

    string[length] = '\0';

    return 0;
}

void reader() {
    char *tmp_message = "[Main thread start]: you can enter your strings now\n";
    write(STDOUT_FILENO, tmp_message, strlen(tmp_message));

    pthread_mutex_lock(&mutx1);

    while (1) {
        input_str_len = read(0, input_str_buf, BUF_SIZE - 1);
        if (command_flag == 1) {
            command_reader();
            command_flag = 0;
            continue;
        }

        if (input_str_len == 0) {
            break;
        }

        int is_normalized = str_normalize_enter(input_str_buf);

        input_str_len -= is_normalized;

        if (input_str_len == 0) {
            continue;
        }

        pthread_cond_signal(&cond_can_first_handle);

        // Проверка условия на входе и выходе - см. конспект в OneNote. и так более наглядно, чего ждем.
        while (input_str_len != 0) {
            pthread_cond_wait(&cond_can_input, &mutx1);
        }
    }

    done = 1;
    pthread_cond_signal(&cond_can_first_handle);

    pthread_mutex_unlock(&mutx1);
}
