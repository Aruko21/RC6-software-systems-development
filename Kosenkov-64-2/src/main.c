/*
 * TODO: Реализовать обработку строк
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#include "general.h"
#include "main_thread.h"
#include "first_thread.h"
#include "second_thread.h"

int done = 0;
int intermediate_str_len = 0;
int input_str_len = 0;

void thread_initialise() {
    pthread_t itid, ctid;
    pthread_attr_t pattr;

    pthread_attr_init(&pattr);
    pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&mutx1, NULL);
    pthread_cond_init(&cond_can_input, NULL);
    pthread_cond_init(&cond_can_first_handle, NULL);
    pthread_mutex_init(&mutx2, NULL);
    pthread_cond_init(&cond_can_first_send, NULL);
    pthread_cond_init(&cond_can_second_handle, NULL);


    if (pthread_create(&itid, &pattr, first_handler, NULL)) {
        perror("pthread_create");
    }
    sleep(1);

    if (pthread_create(&ctid, &pattr, second_handler, NULL)) {
        perror("pthread_create");
    }
}

void thread_free() {
    pthread_mutex_destroy(&mutx1);
    pthread_cond_destroy(&cond_can_input);
    pthread_cond_destroy(&cond_can_first_handle);
    pthread_mutex_destroy(&mutx2);
    pthread_cond_destroy(&cond_can_first_send);
    pthread_cond_destroy(&cond_can_second_handle);
}

int main(int argc, const char **argv) {
    thread_initialise();

    // This interrupts read() function in case of getting signal
    // Linux restarts read() function by default
    struct sigaction s_action;
    // Setting signal handler
    s_action.sa_handler = command_handler;
    // Disable Restart sys_calls flag
    s_action.sa_flags = 0;
    // Initialising sigset structure
    sigemptyset(&s_action.sa_mask);

    if (sigaction(SIGINT, &s_action, NULL) < 0) {
        perror("Sigaction error");
        thread_free();
        pthread_exit(0);
    }

    // Or siginterrupt function can be used instead
    // signal(SIGINT, command_handler);
    // siginterrupt(SIGINT, 1);

    modes[0] = modes[1] = 1;

    reader();

    thread_free();
    pthread_exit(0);
}