#ifndef LAB_DEVSSYS_GENERAL_H
#define LAB_DEVSSYS_GENERAL_H

#include <pthread.h>

#define BUF_SIZE 256

pthread_mutex_t mutx1, mutx2;
pthread_cond_t cond_can_input;
pthread_cond_t cond_can_second_handle;
pthread_cond_t cond_can_first_handle;
pthread_cond_t cond_can_first_send;

extern int input_str_len;
extern int intermediate_str_len;
extern int done;
char input_str_buf[BUF_SIZE];
char intermediate_str_buf[BUF_SIZE];
int modes[2];

void thread_free();
void thread_initialise();

#endif //LAB_DEVSSYS_GENERAL_H
