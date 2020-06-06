cmake_minimum_required(VERSION 3.13)
project(LAB_DEVSSYS C)

set(CMAKE_C_STANDARD 99)
find_package(Threads REQUIRED)

add_executable(lab1 Kosenkov-64-1/src/main.c)
add_executable(lab1-modified Kosenkov-64-1/src/main-modified.c)


set(lab2_include "Kosenkov-64-2/include")
add_executable(lab2 Kosenkov-64-2/src/main.c Kosenkov-64-2/src/first_thread.c
                    Kosenkov-64-2/src/second_thread.c Kosenkov-64-2/src/main_thread.c
                    Kosenkov-64-2/src/string_handlers.c)
target_include_directories(lab2 PRIVATE ${lab2_include})
target_link_libraries(lab2 Threads::Threads)

set(lab3_include "Kosenkov-64-3/include")
add_executable(lab3 Kosenkov-64-3/src/main.c Kosenkov-64-3/src/buffers.c Kosenkov-64-3/src/parsers.c Kosenkov-64-3/src/http_client.c)
target_include_directories(lab3 PRIVATE ${lab3_include})

add_executable(lab4 Kosenkov-64-4/src/main.c)

include_directories(/usr/include/mpi/)

add_executable(atasunts examples/lab2/done_lab_25/main.c examples/lab2/done_lab_25/definitions.h examples/lab2/done_lab_25/st.c examples/lab2/done_lab_25/ft.c)

add_executable(chernov examples/lab3/Chernov/main.c)
add_executable(bobkov examples/lab3/Bobkov/main.c)
add_executable(tretyakova examples/lab3/Tretyakova/main.c)

add_executable(lubyanov examples/lab4/Lubyanov/main.c)
add_executable(kizimenko examples/lab4/Kizimenko/main.c)