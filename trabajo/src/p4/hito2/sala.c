#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sala.h"

int *sala = NULL;
int asientos = 0;
pthread_mutex_t mutex;

int crea_sala(int capacidad) {
    if (sala != NULL) {
        elimina_sala();
    }
    if (capacidad <= 0) {
        return -1;
    }
    sala = (int *)malloc(capacidad * sizeof(int));
    if (sala == NULL) {
        return -1;
    }
    asientos = capacidad;
    for (int i = 0; i < capacidad; i++) {
        sala[i] = -1;
    }
    return 0;
}

int reserva_asiento(int id_persona) {
    if (id_persona <= 0 || asientos_libres() == 0 || asientos == 0) {
        return -1;
    }

	pthread_mutex_lock(&mutex);
    for (int i = 0; i < asientos; i++) {
        if (sala[i] == -1) {
            sala[i] = id_persona;
            pthread_mutex_unlock(&mutex);
            return i + 1;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

int libera_asiento(int id_asiento) {
    if (id_asiento <= 0 || id_asiento > asientos || sala[id_asiento - 1] == -1) {
        return -1; // El asiento ya está libre o el índice es inválido
    }
    pthread_mutex_lock(&mutex);
    int result = sala[id_asiento - 1];
    sala[id_asiento - 1] = -1; // Marcar el asiento como libre
    pthread_mutex_unlock(&mutex);
    return result;
}

int estado_asiento(int id_asiento) {
    pthread_mutex_lock(&mutex);
    if (id_asiento <= 0 || id_asiento > asientos) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    int estado = sala[id_asiento - 1];
    pthread_mutex_unlock(&mutex);
    return (estado == -1) ? 0 : estado;
}

int asientos_libres() {
    pthread_mutex_lock(&mutex);
    int asientos_libres_result = 0;
    for (int i = 0; i < asientos; i++) {
        if (sala[i] == -1) {
            asientos_libres_result++;
        }
    }
    pthread_mutex_unlock(&mutex);
    return asientos_libres_result;
}

int asientos_ocupados() {
    return capacidad_sala() - asientos_libres();
}

int capacidad_sala() {
    return asientos;
}

int elimina_sala() {
    pthread_mutex_lock(&mutex);
    free(sala);
    sala = NULL;
    asientos = 0;
    pthread_mutex_unlock(&mutex);
    return 0;
}

void imprimir() {
    for (int i = 0; i < asientos; i++) {
        printf("%d ", sala[i]);
    }
    printf("\n");
}

void estado_sala() {
    
    printf("Estado de la sala:\n");
    for (int i = 1; i <= capacidad_sala(); i++) {
        int estado = estado_asiento(i);
        if (estado < 0) {
            printf("Error en asiento %d\n", i);
        } else if (estado == 0) {
            printf("El asiento %d esta libre\n", i);
        } else {
            printf("Asiento %d: ocupado por ID %d\n", i, estado);
        }
    }
    printf("Aforo: %d, Ocupacion: %d, Asientos libres: %d\n", capacidad_sala(), asientos_ocupados(), asientos_libres());
    imprimir();
}
