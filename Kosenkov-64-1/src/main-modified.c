#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void exit_handler();
void switch_handler();

char main_loop_flag = 1;
char refresh_speed = 1;
char update_flag = 0;

int main(int argc, const char **argv) {
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    int pid = 0;
    int ppid = getpid();

    int fds[2];
    pipe(fds);

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(-1);
    }

    if (pid != 0) { /* parent */
        close(fds[1]);
        signal(SIGUSR1, exit_handler);
        signal(SIGUSR2, switch_handler);
        char output[32];
        while (main_loop_flag) {
            if (update_flag) {
                char read_status = read(fds[0], &refresh_speed, 1);
                if (read_status != 1) {
                    perror("read from pipe");
                    main_loop_flag = 0;
                    kill(pid, SIGKILL);
                }
                update_flag = 0;
            }

            sprintf(output, "Alarm after %d seconds\n", refresh_speed);
            write(STDOUT_FILENO, output, strlen(output));
            sleep(refresh_speed);
        }
        close(fds[0]);

    } else { /* child */
        close(fds[0]);
        char input = 0;
        char listen_flag = 1;
        char old_interval = refresh_speed;
        char new_interval = 0;
        while (listen_flag) {
            char is_not_eof = 1;
            is_not_eof = read(STDIN_FILENO, &input, 1);

            if (!is_not_eof) {
                kill(ppid, SIGUSR1);
                listen_flag = 0;
            }
            if (input >= '1' && input <= '9') {
                /* char code to digit */
                new_interval = input - '0';
            } else if (input != '\n') {
                new_interval = old_interval;
            }
            if (input == '\n') {
                old_interval = new_interval;
                write(fds[1], &new_interval, 1);
                kill(ppid, SIGUSR2);
            }
        }
        close(fds[1]);
    }
    return 0;
}

void exit_handler() {
    main_loop_flag = 0;
}

void switch_handler() {
    signal(SIGUSR2, switch_handler); /* reset signal */
    update_flag = 1;
}