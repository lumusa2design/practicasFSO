#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int *sala;
int asientos;

int guarda_estado_sala(char* ruta_fichero) {
    FILE *archivo = fopen(ruta_fichero, "w");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return -1;
    }

    // Calcular el número de asientos ocupados y libres
    int ocupados = asientos_ocupados();
    int libres = asientos_libres();

    // Escribir el número de asientos ocupados y libres
    fprintf(archivo, "Asientos ocupados: %d\n", ocupados);
    fprintf(archivo, "Asientos libres: %d\n", libres);
    fprintf(archivo, "Capacidad de la sala: %d\n\n", capacidad_sala());

    // Escribir el estado de cada asiento
    fprintf(archivo, "Estado de los asientos:\n");
    for (int i = 0; i < capacidad_sala(); i++) {
        fprintf(archivo, "Asiento %d: ", i);
        int estado = estado_asiento(i);
        if (estado == -1) {
            fprintf(archivo, "No existe\n");
        } else if (estado == 0) {
            fprintf(archivo, "Libre\n");
        } else {
            fprintf(archivo, "Ocupado (id: %d)\n", estado);
        }
    }

    fclose(archivo);
    return 0;
}

int crea_sala(int capacidad) {
    if (capacidad <= 0) {
        return -1;
    }
    sala = (int *)malloc(capacidad * sizeof(int));
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

    for (int i = 0; i < asientos; i++) {
        if (sala[i] == id_persona) {
            return -1;
        }
        if (sala[i] == -1) {
            sala[i] = id_persona;
            return i;
        }
    }
    return -1;
}


int libera_asiento(int id_asiento) {
    if (sala[id_asiento] == -1) {
        return -1; // El asiento ya está libre
    }
    int result = sala[id_asiento];
    sala[id_asiento] = -1; // Marcar el asiento como libre
    return result;
}


int estado_asiento(int id_asiento) {
    if (id_asiento < 0 || id_asiento >= asientos) {
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
    for (int i = 0; i < asientos; i++) {
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
    sala = NULL; // Establecer sala en NULL después de liberar la memoria
    asientos = 0; // Restablecer el número de asientos a 0
    return 0; // Indicar que la eliminación de la sala fue exitosa
}


void imprimir() {
    for (int i = 0; i < asientos; i++) {
        printf("%d", sala[i]);
    }
    printf("\n");
}