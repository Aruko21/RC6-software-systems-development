#define GRAPHIC_MODE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 256

#define Ia 10.0
#define R 1000.0
#define C 1e-5

int time_interval = 0;
int elements_count = 0;
int elements_height = 0;
int elements_width = 0;
FILE *gp = NULL;

double time_step = R * C / 10;


int main(int argc, char **argv) {
    double *N_matrix = NULL;
    int elements_info[4];
    int rank = 0;
    int total = 0;

    char msg_buf[BUF_SIZE];

    /* Инициализация библиотеки MPI, передача в инициализирующую функцию аргументов
     * командной строки, определяющих количество подпрограмм
     */
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &total);  // Количество процессов в группе
    MPI_Comm_rank(MPI_COMM_WORLD,
                  &rank); // Номер конкретного процесса (на данном этапе данный код исполняется в разных процессах


    if (rank == 0) {
        // root-процесс
        if (argc < 4) {
            sprintf(msg_buf, "Incorrect arguments. Usage: %s <width> <height> <time_n>\n", argv[0]);
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));

            exit(-1);
        }

#ifdef GRAPHIC_MODE
        gp = fopen("results.gp", "w");
#endif

        int proc_count = total;

        time_interval = atoi(argv[3]);
        elements_height = atoi(argv[2]);
        elements_width = atoi(argv[1]);

        if (elements_width > elements_height && elements_width % proc_count == 0) {
            // Оптимизация с целью уменьшения количества элементов по горизонтали
            int tmp_height = elements_width;
            elements_width = elements_height;
            elements_height = tmp_height;
        }

        elements_count = elements_height * elements_width;

        if (elements_height % proc_count != 0) {
            sprintf(msg_buf, "Number of processes is not multiple to elements\n");
            write(STDOUT_FILENO, msg_buf, strlen(msg_buf));
            exit(0);
        }

        elements_info[0] = elements_width; //количество элементов в строке
        elements_info[1] = elements_height / proc_count; //количество строк на процесс
        elements_info[2] = proc_count; // количество процессов
        elements_info[3] = time_interval; //время работы (в миллисекундах)

        N_matrix = (double *) malloc(elements_count * sizeof(double)); // итоговая матрица
        for (int i = 0; i < elements_count; i++) {
            N_matrix[i] = 0.0;
        }
    }

    /* Уведомление других процессов об информации о количестве элементов и процессов */
    MPI_Bcast((void *) elements_info, 4, MPI_INT, 0, MPI_COMM_WORLD);

    int cols = elements_info[0];
    int rows = elements_info[1];
    int count = elements_info[2];
    time_interval = elements_info[3];

    double *el_prev = (double *) malloc(cols * sizeof(double)); // элементы последней строки предшествующей ленты
    double *el_next = (double *) malloc(cols * sizeof(double)); // элементы первой строки следующей ленты
    double *strip_n = (double *) malloc(rows * cols * sizeof(double)); // матрица текущего момента ленты
    double *strip_np1 = (double *) malloc(cols * rows * sizeof(double)); // матрица следующего момента ленты

    // Инициализация лент
    MPI_Scatter((void *) N_matrix, cols * rows, MPI_DOUBLE, (void *) strip_n, cols * rows, MPI_DOUBLE, 0,
                MPI_COMM_WORLD);

    for (int time = 0; time < time_interval / (time_step * 1000); ++time) {
        MPI_Status *status1 = (MPI_Status *) malloc(sizeof(MPI_Status));
        MPI_Status *status2 = (MPI_Status *) malloc(sizeof(MPI_Status));

        if (rank != count - 1) {    // Если не последний процесс
            int next_strip = rank + 1; // следующая лента

            /* Отправка последней строки текущей ленты следующей ленте */
            MPI_Send(strip_n + cols * (rows - 1), // буфер с данными
                     cols, // количество элементов сообщения
                     MPI_DOUBLE, // тип данных
                     next_strip, // ид ветви
                     0, // ид сообщения
                     MPI_COMM_WORLD // область связи (коммутатор)
            );

            /* Прием первой строки следующей ленты */
            /* Последний аргумент - информация о принятом сообщении */
            MPI_Recv(el_next, cols, MPI_DOUBLE, next_strip, 0, MPI_COMM_WORLD, status2);
        }

        if (rank != 0) {
            int prev_strip = rank - 1;   // предыдущеая лента

            /* Отправка первой строки текущей ленты предыдущей ленте */
            MPI_Send(strip_n, cols, MPI_DOUBLE, prev_strip, 0, MPI_COMM_WORLD);
            /* Преим последней строки предыдущей ленты */
            MPI_Recv(el_prev, cols, MPI_DOUBLE, prev_strip, 0, MPI_COMM_WORLD, status1);
        }

        free(status1);
        free(status2);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if ((rank == 0 && i == 0) || ((rank == count - 1) && (i == rows - 1)) || (j == 0) ||
                    (j == cols - 1)) { // граничные узлы
                    strip_np1[i * cols + j] = Ia * R;
                } else { // внутренние узлы
                    if (i == 0) { // верхняя полоса
                        strip_np1[i * cols + j] =
                                (strip_n[i * cols + (j - 1)] - 4 * strip_n[i * cols + j] + strip_n[i * cols + (j + 1)] +
                                 strip_n[(i + 1) * cols + j] + el_prev[j]) * time_step / (R * C) +
                                strip_n[i * cols + j];

                    } else if (i == rows - 1) { // нижняя полоса
                        strip_np1[i * cols + j] =
                                (strip_n[i * cols + (j - 1)] - 4 * strip_n[i * cols + j] + strip_n[i * cols + (j + 1)] +
                                 el_next[j] + strip_n[(i - 1) * cols + j]) * time_step / (R * C) +
                                strip_n[i * cols + j];

                    } else { // средние полосы
                        strip_np1[i * cols + j] =
                                (strip_n[i * cols + (j - 1)] - 4 * strip_n[i * cols + j] + strip_n[i * cols + (j + 1)] +
                                 strip_n[(i + 1) * cols + j] + strip_n[(i - 1) * cols + j]) * time_step / (R * C) +
                                strip_n[i * cols + j];
                    }
                }
            }
        }

#ifdef GRAPHIC_MODE
        MPI_Gather((void *) strip_np1, // буфер, из которого отправляются данные
                   rows * cols, // кол-во элементов отправляемых данных
                   MPI_DOUBLE, // тип отправляемых данных
                   (void *) N_matrix, // буфер для размещения собранных данных
                   rows * cols, // сколько элементов принимает собирающий процесс
                   MPI_DOUBLE, // тип собираемых данных
                   0, // куда отсылать, кто принимает
                   MPI_COMM_WORLD // процессы из какой группы выполняют эту функцию
        );
#endif

        /* n + 1-я итерация далее становится n-й */
        double *temp_ptr = strip_n;
        strip_n = strip_np1;
        strip_np1 = temp_ptr;

#ifdef GRAPHIC_MODE
        if (rank == 0) {
            // вывод графика
            fprintf(gp, "splot [][][0:10000] '-' title 'time = %.3lf sec'\n", time * time_step);

            for (int k = 0; k < elements_count; ++k) {
                fprintf(gp, "%# -15g%# -15g%# -15g\n", (double) (k / elements_width), (double) (k % elements_width),
                        N_matrix[k]);
            }

            fprintf(gp, "e\n");
            fprintf(gp, "pause 0.01 \n");
            fflush(gp);
        }
#endif
    }

    if (rank == 0) {
        free(N_matrix);
        fclose(gp);
    }

    free(el_prev);
    free(el_next);
    free(strip_n);
    free(strip_np1);

    MPI_Finalize();
    exit(0);
}
