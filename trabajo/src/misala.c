#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "sala.h"
#include "salacreate.c"
#include "salareservation.c"
#include "salacancel.c"
#include "salastate.c"

int aux = 1;
int check_ruta(const char *ruta) {
    if (access(ruta, F_OK) == -1) {
        return -1;
    } else {
        int permisos = 0;
        if (access(ruta, R_OK) == 0) {
            permisos += 4;
        }
        if (access(ruta, W_OK) == 0) {
            permisos += 2;
        }
        return permisos;
    }
}

void check_error(int result, const char *error_message) {
    if (result < -1) {
        print_error(error_message);
        exit(EXIT_FAILURE);
    }
}

int verificar_permisos_F_OK_R_OK(const char* ruta){
    if(access(ruta, F_OK) == 0){
    	if (access(ruta, R_OK) == -1){
    		print_error("No se posee permiso de lectura en la ruta indicada\n");
    		exit(EXIT_FAILURE);
    		return -1;
    	}
    	return 0;
    } else {
    	print_error("Ruta no válida\n");
    	return -1;
    }
}

int* recupera_estado_asientos(const char* ruta, int capacidad){
    int* estados_asientos = NULL;

    if (crea_sala(capacidad) == -1 || recupera_estado_sala(ruta) == -1) {
        print_error("Hubo un error al crear o recuperar la sala");
        exit(EXIT_FAILURE);
    }

    estados_asientos = malloc(capacidad * sizeof(int));
    if (estados_asientos == NULL) {
        elimina_sala();
        print_error("No se pudo asignar memoria para almacenar los estados de los asientos para la ruta indicada.\n");
        exit(EXIT_FAILURE);
    }

    for (int pos = 0; pos < capacidad; pos++){
        estados_asientos[pos] = estado_asiento(pos+1);
    }
    elimina_sala();
    return estados_asientos;
}

int obtener_capacidad_archivo(const char* ruta){
    int fid = open(ruta, O_RDONLY);
    
    if(fid == -1){
        print_error("No se pudo abrir el archivo para leer\n");
        exit(EXIT_FAILURE);
        return -1;
    }
   
    int capacidad_leida;
    if (read(fid, &capacidad_leida, sizeof(int)) == -1) {
        print_error("Error al leer la capacidad del archivo\n");
        exit(EXIT_FAILURE);
        return -1;
    }
    
    if (capacidad_leida < 0){
        print_error("El archivo no guarda la información de una sala\n");
        exit(EXIT_FAILURE);
        return -1;
    } else {
        return capacidad_leida;
    } 
}

int comprueba_si_fichero_contiene_info_sala(const char* ruta){
    int num_bytes_asientos;	
    struct stat st;
    if (stat(ruta, &st) == -1) {
        print_error("Error al obtener la información del archivo");
        exit(EXIT_FAILURE);
        return -1;
    }
    
    int capacidad_sala = obtener_capacidad_archivo(ruta);
    num_bytes_asientos = (long long)st.st_size - sizeof(int);
    
    if ((capacidad_sala * sizeof(int)) == num_bytes_asientos){
    	return capacidad_sala;
    } else {
    	fprintf(stderr, "El archivo '%s' no contiene la información de una sala\n", ruta);
    	exit(EXIT_FAILURE);
    	return -1;
    }
}

int compara_misala(int argc, char* argv[]){
    if (argc != 4){
        print_error("Error en el comando\n");
    }
    
    char *ruta1 = argv[2];
    char *ruta2 = argv[3];
    
    if (verificar_permisos_F_OK_R_OK(ruta1) == -1 || verificar_permisos_F_OK_R_OK(ruta2) == -1){
        return -1;
    }
    
    int capacidad_sala_1 = comprueba_si_fichero_contiene_info_sala(ruta1);
    int capacidad_sala_2 = comprueba_si_fichero_contiene_info_sala(ruta2);
    if(capacidad_sala_1 == -1 || capacidad_sala_2 == -1){
        return -1;
    }
    
    if (capacidad_sala_1 != capacidad_sala_2){
        printf("Las salas tienen capacidades distintas\n");
        return 1;
    }
    
    int *asientos_sala_1 = recupera_estado_asientos(ruta1, capacidad_sala_1);
    int *asientos_sala_2 = recupera_estado_asientos(ruta2, capacidad_sala_2);
    
    for (int comp = 0; comp < capacidad_sala_1; comp++){
        if(asientos_sala_1[comp] != asientos_sala_2[comp]){
            printf("Las salas tienen la misma capacidad, pero el estado de los asientos es distinto\n");
            return 1;
        }
    }
    
    printf("Las salas son idénticas\n");
    return 0;
}

int main(int argc, char* argv[])
{ 
    if(strcmp(argv[1],"crea") == 0){
        if (crear_misala(argc, argv) == 0){
            printf("Sala creada y guardada con éxito en '%s'\n", argv[3]);
        }
    } else if(strcmp(argv[1],"reserva") == 0){
        if (reserva_misala(argc, argv) == 0) {
            printf("Reserva exitosa\n");
        }       	
    } else if(strcmp(argv[1], "estado")==0){
        estado_misala(argc, argv);	
    } else if(strcmp(argv[1], "anula") == 0){
        anula_misala(argc, argv);
    } else if(strcmp(argv[1], "compara") == 0){
        compara_misala(argc, argv);	
    } else {
        print_error("Comando inválido, información usando './misala help'\n");
    }
  	
    return 0;
}

