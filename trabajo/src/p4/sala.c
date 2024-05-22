#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "sala.h"
#include <string.h>

int *sala;
int asientos;

int crea_sala(int capacidad) {
	
	if (sala!=NULL){
	elimina_sala();
	}
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
    if (id_asiento < -1 || id_asiento > asientos) {
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

void estado_sala() {
	printf("Estado de la sala:\n");
	for (int i = 0; i <= capacidad_sala(); i++){
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

int guarda_estado_sala(const char* ruta_fichero) {
    int fd = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error al abrir el fichero");
        return -1;
    }

    char capacidad_str[2000];
    int capacidad_length = sprintf(capacidad_str, "%d\n", asientos);
    write(fd, capacidad_str, capacidad_length);

    for (int i = 0; i < asientos; i++) {
        char asiento_str[2000];
        int asiento_length = sprintf(asiento_str, "%d ", sala[i]);
        write(fd, asiento_str, asiento_length);
    }
    capacidad_length = sprintf(capacidad_str, "\n");
    write(fd, "\n", capacidad_length);
    close(fd);
    return 0;
}

int recupera_estado_sala(const char* ruta_fichero) {
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return -1;
    }

    int capacidad_recuperada = 0;
    char buffer[10000];
    
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Error al leer la capacidad de la sala desde el archivo");
        close(fd);
        return -1;
    }
    sscanf(buffer, "%d", &capacidad_recuperada);
    
    crea_sala(capacidad_recuperada);

    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Error al leer el estado de los asientos desde el archivo");
        close(fd);
        return -1;
    }

    int estado_asiento = 0;
    char* ptr = buffer;
    int i = 0;
    
    while (*ptr != ' ' && *ptr != '\n') {
        ptr++;
    }
    while (*ptr == ' ') {
        ptr++;
    }

    while (*ptr != '\0' && i < capacidad_recuperada) {
        if (sscanf(ptr, "%d", &estado_asiento) != 1) {
            perror("Error al leer el estado de los asientos");
            close(fd);
            return -1;
        }
        sala[i++] = estado_asiento;
        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
        while (*ptr == ' ') {
            ptr++;
        }
    }

    close(fd);
    return 0;
}

int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    if (sala == NULL) {
        return -1;
    }

    int fd = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Error al abrir el fichero");
        return -1;
    }

    char buffer[100];
    ssize_t bytes_written = snprintf(buffer, sizeof(buffer), "%d\n", capacidad_sala());
    write(fd, buffer, bytes_written);

    for (size_t i = 0; i < num_asientos; i++) {
		int asiento = id_asientos[i];
		
		if(estado_asiento(asiento) == 0) {
			bytes_written = snprintf(buffer, sizeof(buffer), "0 ", asiento);
			write(fd, buffer, bytes_written);
		} else {
			bytes_written = snprintf(buffer, sizeof(buffer), "%d ", asiento, estado_asiento(asiento));
			write(fd, buffer, bytes_written);
		}
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "\n");
    write(fd, buffer, bytes_written);

    close(fd);
    return 0;
}

int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return -1;
    }

    int capacidad_recuperada = 0;
    char buffer[10000];
    
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Error al leer la capacidad de la sala desde el archivo");
        close(fd);
        return -1;
    }
    sscanf(buffer, "%d", &capacidad_recuperada);

    bytes_read = read(fd, buffer, sizeof(buffer));
    if (bytes_read == -1) {
        perror("Error al leer el estado de los asientos desde el archivo");
        close(fd);
        return -1;
    }

    int estado_asiento = 0;
    char* ptr = buffer;
    int i = 0;
    
    while (*ptr != ' ' && *ptr != '\n') {
        ptr++;
    }
    while (*ptr == ' ') {
        ptr++;
    }

    while (*ptr != '\0' && i < num_asientos) {
        if (sscanf(ptr, "%d", &estado_asiento) != 1) {
            perror("Error al leer el estado de los asientos");
            close(fd);
            return -1;
        }
        id_asientos[i++] = estado_asiento;
        while (*ptr != ' ' && *ptr != '\0') {
            ptr++;
        }
        while (*ptr == ' ') {
            ptr++;
        }
    }

    close(fd);
    return 0;
}

