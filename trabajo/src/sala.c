#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int *sala;
int asientos;

int reservar_asiento_en_sala_llena;

int crea_sala(int capacidad) {
    if (capacidad <= 0) 
        return -1;
        
    sala = (int *)malloc(capacidad * sizeof(int));
    asientos = capacidad;
    for (int i = 0; i < capacidad; i++) 
        sala[i] = -1;
        
    return 0;
}

int reserva_asiento(int id_persona) {
    if (id_persona <= 0 || asientos_libres() == 0 || asientos == 0) 
        return -1;

    for (int i = 0; i < asientos; i++) {
        if (sala[i] == id_persona) 
            return -1;
   
        if (sala[i] == -1) {
            sala[i] = id_persona;
            return i;
        }
    }
    return -1;
}


int libera_asiento(int id_asiento) {
    if (sala[id_asiento] == -1) {
        return -1; 
    }
    int result = sala[id_asiento];
    sala[id_asiento] = -1; 
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
    sala = NULL; 
    asientos = 0; 
    return 0; 
}


void imprimir() {
    for (int i = 0; i < asientos; i++) {
        printf("%d", sala[i]);
    }
    printf("\n\n");
}


