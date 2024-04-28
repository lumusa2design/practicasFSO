#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int* ptr;
int capacidad_main;
int asientos_ocu;



// Recupera el estado completo de la sala desde un archivo
int recupera_estado_sala(const char* ruta_fichero) {
    if (existe_sala() == -1) {
        return -1;
    }

    int fid = open(ruta_fichero, O_RDONLY);
    if (fid == -1) {
        print_error("No se pudo abrir el archivo para leer");
        return -1;
    }

    int capacidad_leida;
    if (read(fid, &capacidad_leida, sizeof(int)) == -1) {
        print_error("Error al leer la capacidad del archivo");
        close(fid);
        return -1;
    }

    if (capacidad_leida != capacidad_main) {
        print_error("El estado de la sala que se ha intentado recuperar no coincide en capacidad con la sala actual");
        close(fid);
        return -1;
    }

    struct stat fileStat;
    if (fstat(fid, &fileStat) == -1) {
        print_error("Error al obtener información del archivo");
        close(fid);
        return -1;
    }

    int capacidad_en_bytes = capacidad_main * sizeof(int);
    int num_bytes_block = fileStat.st_blksize;
    int num_iteraciones_lectura = capacidad_en_bytes / num_bytes_block;
    int resto = capacidad_en_bytes % num_bytes_block;

    for (int i = 0; i < num_iteraciones_lectura; i++) {
        if (read(fid, ptr, num_bytes_block) == -1) {
            print_error("Error al leer del archivo");
            close(fid);
            return -1;
        }
    }

    if (resto > 0) {
        if (read(fid, ptr, resto) == -1) {
            print_error("Error al leer del archivo");
            close(fid);
            return -1;
        }
    }

    asientos_ocu = 0;
    for (int i = 0; i < capacidad_main; i++) {
        if (*(ptr + i) > -1) {
            asientos_ocu += 1;
        }
    }

    if (close(fid) == -1) {
        print_error("Error al cerrar el archivo");
        return -1;
    }

    return 0;
}

int guarda_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    struct stat fileStat;
    if (stat(ruta_fichero, &fileStat) == -1) {
        print_error("Error al obtener información del archivo");
        return -1;
    }

    int fid = open(ruta_fichero, O_WRONLY, fileStat.st_mode);
    if (fid == -1) {
        print_error("No se pudo abrir el archivo para escribir");
        return -1;
    }

    int numero_escribir;
    for (size_t i = 0; i < num_asientos; i++) {
        if (lseek(fid, (*(id_asientos + i) * sizeof(int)), SEEK_SET) == -1) {
            print_error("Error al posicionar el puntero en el archivo");
            close(fid);
            return -1;
        }
        if (*(id_asientos + i) < capacidad_main || *(id_asientos + i) >= 0) {
            numero_escribir = *(ptr + *(id_asientos + i) - 1);
            if (write(fid, &numero_escribir, sizeof(numero_escribir)) == -1) {
                print_error("Error al escribir en el archivo");
                close(fid);
                return -1;
            }
        }
    }

    if (close(fid) == -1) {
        print_error("Error al cerrar el archivo");
        return -1;
    }

    return 0;
}

int recupera_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
    int fid = open(ruta_fichero, O_RDONLY);
    if (fid == -1) {
        print_error("No se pudo abrir el archivo para leer");
        return -1;
    }

    int numero_a_leer;
    for (size_t i = 0; i < num_asientos; i++) {
        if (lseek(fid, *(id_asientos + i) * sizeof(int), SEEK_SET) == -1) {
            print_error("Error al posicionar el puntero en el archivo");
            close(fid);
            return -1;
        }
        if (read(fid, &numero_a_leer, sizeof(int)) == -1) {
            print_error("Error al leer del archivo");
            close(fid);
            return -1;
        }
        if (*(id_asientos + i) < capacidad_main || *(id_asientos + i) >= 0) {
            if (numero_a_leer > -1 && *(ptr + *(id_asientos + i)) == -1) {
                asientos_ocu += 1;
            } else if (numero_a_leer == -1 && *(ptr + *(id_asientos + i)) > -1) {
                asientos_ocu -= 1;
            }
            *(ptr + *(id_asientos + i) - 1) = numero_a_leer;
        }
    }

    if (close(fid) == -1) {
        print_error("Error al cerrar el archivo");
        return -1;
    }

    return 0;
}

