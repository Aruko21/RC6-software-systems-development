FROM ubuntu:16.04

WORKDIR /home/lab2

COPY . .
RUN apt update -y
RUN apt install -y make gcc
RUN make clean && make lab2

MAINTAINER Aleksandr Kosenkov

CMD ./lab2