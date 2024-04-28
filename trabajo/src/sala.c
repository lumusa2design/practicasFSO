#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>


#include <stdio.h>
#include <errno.h>


#include <unistd.h>
#include <fcntl.h>

int *sala;
int asientos;

int guarda_estado_sala( char* ruta)
{
    ///Abrimos el fichero
    int fid = open(ruta, O_WRONLY);

    ///Comprobamos que se ha creado bien
    if (fid == -1)
    {
        comprueba_error();
        return -1;
    }

    ///Creamos el buffer
    char buf[320];

    ///Comprobamos qeu se creó bien
    if (buf == NULL){
        comprueba_error();
        return -1;
    }

    /// Sobreescribimos el numero de asientos de la sala
    int len = sprintf(buf, "Capacidad de la sala: %d", capacidad_sala());
    lseek(fid, -1*sizeof(char), SEEK_CUR);
    write(fid, buf, len+1);


    /// Sobreescribimos el numero de asientos reservados de la sala
    len = sprintf(buf, "\nNúmero de asientos reservados: %d\n", asientos_ocupados());
    lseek(fid, -1*sizeof(char), SEEK_CUR);
    write(fid, buf, len);

    /// Sobreescribimos el numero de asientos libres de la sala
    len = sprintf(buf, "\nNúmero de asientos libres: %d\n\nSala: ", asientos_libres());
    lseek(fid, -1*sizeof(char), SEEK_CUR);
    write(fid, buf, len);

    /// Sobreescribimos el vector de los asientos de la sala
    for (int i = 0; i < sizeof(sala); i++) {
        if (i != sizeof(sala))
        {
            len = sprintf(buf, "%d ", sala[i]);
            write(fid, buf, len);
        }
    }

    ///Cerramos el fichero
    close(fid);

    ///Si el fichero se cierra mal devuelve -1
    if (close(fid) == -1)
    {
        comprueba_error();
        printf("mal");
        return -1;
    }

    ///Si todo sale bien devuelve 0
    return 0;
}
int comprueba_error()
{
    if (errno!=0)
    {
        fprintf(stderr, "ERROR con codigo %d: %s\n", errno, strerror(errno));
        errno =0; //Siempre igualar a 0 si no cuando se vuelva a llamar sigue valiendo lo de antes
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