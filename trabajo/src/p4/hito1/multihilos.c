#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sala.h"

#define DELAY_TIME 100 

void retardo_milisegundos(int milisegundos) {
    usleep(milisegundos * 1000);
}

void* reserva_y_libera(void* id) {
    int hilo_id = *((int*)id);
    int asientos_reservados[3];

    for (int i = 0; i < 3; i++) {
        retardo_milisegundos(DELAY_TIME);
        int asiento = reserva_asiento(hilo_id);
        if (asiento == -1) {
            printf("Hilo %d: No se pudo reservar asiento\n", hilo_id);
            pthread_exit(NULL);
        }
        asientos_reservados[i] = asiento;
        printf("Hilo %d: Asiento %d reservado\n", hilo_id, asiento);
    }

    for (int i = 0; i < 3; i++) {
        retardo_milisegundos(DELAY_TIME);
        int asiento_liberado = libera_asiento(asientos_reservados[i]);
        if (asiento_liberado == -1) {
            printf("Hilo %d: Error al liberar asiento %d\n", hilo_id, asientos_reservados[i]);
            pthread_exit(NULL);
        }
        printf("Hilo %d: Asiento %d liberado\n", hilo_id, asientos_reservados[i]);
    }

    pthread_exit(NULL);
}

void* muestra_estado(void* arg) {
    while (1) {
        retardo_milisegundos(1500);
        estado_sala();
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <numero_de_hilos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "El número de hilos debe ser un entero positivo\n");
        exit(EXIT_FAILURE);
    }

    crea_sala(25);

    pthread_t hilos[n];
    pthread_t hilo_estado;
    int ids[n];

    pthread_create(&hilo_estado, NULL, muestra_estado, NULL);

    for (int i = 0; i < n; i++) {
        ids[i] = i + 1;
        pthread_create(&hilos[i], NULL, reserva_y_libera, &ids[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_cancel(hilo_estado);
    pthread_join(hilo_estado, NULL);

    elimina_sala();
    return 0;
}

