#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sala.h"
#include "retardo.h"

#define NUM_ASIENTOS 250
#define NUM_RESERVAS 3
#define MAX_PAUSA_SEGUNDOS 0.25

// Función para reserva y liberación de asientos
void* reserva_y_libera(void* arg) {
    int id_hilo = *((int*)arg);
    int asientos_reservados[NUM_RESERVAS];
    
    // Reservar asientos
    for (int i = 0; i < NUM_RESERVAS; ++i) {
        pausa_aleatoria(MAX_PAUSA_SEGUNDOS);
        int asiento = reserva_asiento(id_hilo);
        if (asiento == -1) {
            printf("Hilo %d: No se pudo reservar asiento\n", id_hilo);
            pthread_exit(NULL);
        }
        printf("Hilo %d: Asiento %d reservado\n", id_hilo, asiento);
        asientos_reservados[i] = asiento;
    }

    // Liberar asientos
    for (int i = 0; i < NUM_RESERVAS; ++i) {
        pausa_aleatoria(MAX_PAUSA_SEGUNDOS);
        int resultado = libera_asiento(asientos_reservados[i]);
        if (resultado == -1) {
            printf("Hilo %d: Error al liberar asiento %d\n", id_hilo, asientos_reservados[i]);
            pthread_exit(NULL);
        }
        printf("Hilo %d: Asiento %d liberado\n", id_hilo, asientos_reservados[i]);
    }

    pthread_exit(NULL);
}

void* hilo_estado_sala(void* arg) {
    while (1) {
        pausa_aleatoria(MAX_PAUSA_SEGUNDOS);
        estado_sala();
    }
}

// Función principal
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Por favor, introduzca el número de hilos que quiere lanzar, de la forma: './multihilos x'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_hilos = atoi(argv[1]);
    if (num_hilos <= 0) {
        fprintf(stderr, "El número de hilos debe ser un entero positivo\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar la sala con un número fijo de asientos
    crea_sala(NUM_ASIENTOS);

    // Crear los hilos para reserva y liberación de asientos
    pthread_t hilos[num_hilos];
    for (int i = 0; i < num_hilos; ++i) {
        int* id_hilo = malloc(sizeof(int));
        if (id_hilo == NULL) {
            fprintf(stderr, "Error al asignar memoria para el ID del hilo\n");
            exit(EXIT_FAILURE);
        }
        *id_hilo = i + 1;
        pthread_create(&hilos[i], NULL, reserva_y_libera, id_hilo);
    }

    // Crear el hilo para imprimir el estado de la sala
    pthread_t hilo_estado;
    pthread_create(&hilo_estado, NULL, hilo_estado_sala, NULL);

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_hilos; ++i) {
        pthread_join(hilos[i], NULL);
    }

    // Eliminar la sala y finalizar
    elimina_sala();
    return 0;
}

