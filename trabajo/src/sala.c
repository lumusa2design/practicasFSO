#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int *sala;
int asientos;

int guarda_estado_sala(char* ruta_fichero) {
   
    int fid = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fid == -1) {
        perror("Error en la apertura del fichero\n");
        return -1;
    }

    // Escribir capacidad de la sala
    char capacidad_str[20];
    int_to_str(capacidad_sala(), capacidad_str);
    int capacidad_length = num_digits(capacidad_sala());
    write(fid, "Capacidad de la sala: ", 22);
    write(fid, capacidad_str, capacidad_length);
    write(fid, "\n", 1);

    // Escribir asientos libres
    char libres_str[20];
    int_to_str(asientos_libres(), libres_str);
    int libres_length = num_digits(asientos_libres());
    write(fid, "Asientos libres: ", 17);
    write(fid, libres_str, libres_length);
    write(fid, "\n", 1);

    // Escribir asientos ocupados
    char ocupados_str[20];
    int_to_str(asientos_ocupados(), ocupados_str);
    int ocupados_length = num_digits(asientos_ocupados());
    write(fid, "Asientos ocupados: ", 19);
    write(fid, ocupados_str, ocupados_length);
    write(fid, "\n", 1);

    // Escribir estado de cada asiento
    for (int i = 0; i < capacidad_sala(); i++) {
        char asiento_str[20];
        int_to_str(i+1, asiento_str);
        int asiento_length = num_digits(i+1);
        write(fid, "\nAsiento ", 9);
        write(fid, asiento_str, asiento_length);
        write(fid, ": ", 2);
        if (sala[i]==-1) {
            write(fid, "Libre", 5);
        } else {
            write(fid, "Ocupado por el id ", 18);
            char info_id[12];
            int_to_str(sala[i], info_id);
            int info_id_length = num_digits(sala[i]);
            write(fid, info_id, info_id_length);
        }
    }

    if (close(fid) == -1) {
        perror("Error al cerrar el archivo");
        return -1;
    }

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