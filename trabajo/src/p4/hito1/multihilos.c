#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

int *sala;
int asientos;

#define NUM_RESERVAS 3

typedef struct {
    int id;
} HiloReserva;

void *reserva_y_libera(void *arg) {
    HiloReserva *hilo = (HiloReserva *)arg;
    int id = hilo->id;
    int asientos_reservados[NUM_RESERVAS];
    srand(time(NULL) + id);

    for (int i = 0; i < NUM_RESERVAS; i++) {
        asientos_reservados[i] = reserva_asiento(id);
        if (asientos_reservados[i] == -1) {
            printf("Hilo %d: No se pudo realizar la reserva\n", id);
        } else {
            printf("Hilo %d: Asiento %d reservado\n", id, asientos_reservados[i]);
        }
        usleep((rand() % 1000 + 500) * 1000);
    }

    for (int i = 0; i < NUM_RESERVAS; i++) {
        int asiento_liberado = libera_asiento(asientos_reservados[i]);
        if (asiento_liberado == -1) {
            printf("Hilo %d: No se pudo liberar el asiento\n", id);
        } else {
            printf("Hilo %d: Asiento %d liberado\n", id, asientos_reservados[i]);
        }
        usleep((rand() % 1000 + 500) * 1000);
    }

    return NULL;
}

void *hilo_mostrar_estado(void *arg) {
    while (1) {
        printf("Estado del asiento: ");
        for (int i = 1; i <= capacidad_sala(); i++) {
            printf("%d ", estado_asiento(i));
        }
        printf("\n");
        usleep(500000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    crea_sala(20);

    if (argc != 2) {
        printf("Error de sintaxis, uso correcto: multihilos n\n");
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Error: Número inválido de hilos\n");
        return 1;
    }

    pthread_t *hilos = malloc(sizeof(pthread_t) * n);
    HiloReserva *hilos_reserva = malloc(sizeof(HiloReserva) * n);
    pthread_t hilo_estado;

    for (int i = 0; i < n; i++) {
        hilos_reserva[i].id = i + 1;
        pthread_create(&hilos[i], NULL, reserva_y_libera, (void *)&hilos_reserva[i]);
    }

    pthread_create(&hilo_estado, NULL, hilo_mostrar_estado, NULL);

    for (int i = 0; i < n; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_cancel(hilo_estado);

    free(hilos);
    free(hilos_reserva);
    
    elimina_sala();

    return 0;
}

int crea_sala(int capacidad) {
    if (sala != NULL) {
        elimina_sala();
    }
    if (capacidad <= 0) {
        return -1;
    }
    sala = (int *)malloc(capacidad * sizeof(int));
    asientos = capacidad;
    for (int i = 1; i <= capacidad; i++) {
        sala[i] = -1;
    }
    return 0;
}

int reserva_asiento(int id_persona) {
    if (id_persona <= 0 || asientos_libres() == 0 || asientos == 0) {
        return -1;
    }

    for (int i = 1; i <= asientos; i++) {
        if (sala[i] == -1) {
            sala[i] = id_persona;
            return i;
        }
    }
    return -1;
}

int libera_asiento(int id_asiento) {
    if (id_asiento <= 0 || id_asiento > asientos) {
        return -1;
    }

    if (sala[id_asiento] != -1) {
        sala[id_asiento] = -1;
        return id_asiento;
    }
    return -1;
}

int estado_asiento(int id_asiento) {
    if (id_asiento < 1 || id_asiento > asientos) {
        return -1;
    }

    if (sala[id_asiento] == -1) {
        return 0;
    } else {
        return sala[id_asiento];
    }
}

int asientos_libres() {
    int asientos_libres_result = 0;
    for (int i = 1; i <= asientos; i++) {
        if (sala[i] == -1) {
            asientos_libres_result++;
        }
    }
    return asientos_libres_result;
}

int asientos_ocupados() {
    return capacidad_sala() - asientos_libres();
}

int capacidad_sala() {
    return asientos;
}

int elimina_sala() {
    free(sala);
    sala = NULL;
    asientos = 0;
    return 0;
}
