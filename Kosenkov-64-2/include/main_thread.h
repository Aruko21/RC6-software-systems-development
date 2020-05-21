#ifndef LAB_DEVSSYS_MAIN_THREAD_H
#define LAB_DEVSSYS_MAIN_THREAD_H

extern char command_flag;

int str_normalize_enter(char *);
void command_handler();
void command_reader();
void reader();

#endif //LAB_DEVSSYS_MAIN_THREAD_H
