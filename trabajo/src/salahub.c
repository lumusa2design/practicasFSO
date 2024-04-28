#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int* ptr;
int capacidad_main;
int asientos_ocu;


void print_error(const char* msg) {
    if (errno != 0) {
        char error_message[256];
        strerror_r(errno, error_message, sizeof(error_message));
        fprintf(stderr, "\nERROR CONTROLADO: %s\n errno number: %d \n errno: %s\n", msg, errno, error_message);
        errno = 0;
    } else {
        fprintf(stderr, "\nERROR CONTROLADO: %s\n", msg);
    }
    fflush(stderr);
}

#include "sala.c"

