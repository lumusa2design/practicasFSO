#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "sala.h"
#include <getopt.h>
#include <ctype.h>

#define MAX_ARGUMENTS 100

int main(int argc, char *argv[]) {
    int aorp = 0;
    int c;
    int capacidad = 0;
    int indice_a = -1;
    int sobrescribir = 0;
    char* orden = NULL;
    char* ruta = NULL;
    
    while ((c=getopt( argc, argv, "f:o:c:p:a" )) !=-1) {
        switch (c) {
            case 'f':
                ruta=optarg;
                break;
            case 'o':
            	ruta=optarg;
                sobrescribir=1;
                break;
            case 'c':
                capacidad=atoi(optarg);
                break;
            case 'p':
            	indice_a=optind -1;
            	aorp=0;
            	break;
            case 'a':
            	indice_a=optind - 1;
            	aorp=1;
            	break;
            case '?':
                if (optopt=='f' || optopt=='c'){
                    fprintf(stderr, "Error: La opción -%c requiere un argumento.\n", optopt);
               
                } else if (isprint(optopt)){
                    fprintf(stderr, "Error: No se recconoce la opción '-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Error: no se reconoce el carácter '\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }
    
    if (optind < argc) {
        orden = argv[optind];
    } else {
        fprintf(stderr, "Por favor, introduzca una orden.\n");
        return 1;
    }
    
    /* CREAR SALA */
    if (strcmp(orden, "crea") == 0) {
    
        if (ruta == NULL) {
		fprintf(stderr, "Por favor, introduzca la ruta del archivo\n");        
        }
        
        if (capacidad <= 0) {
            fprintf(stderr, "Hubo un error al intentar crear la sala. Capacidad no válida.\n");
            return 1;
        }
        
        int fd = open(ruta, O_RDONLY);
        if (fd != -1) {
            close(fd);
            if (!sobrescribir) {
        	fprintf(stderr, "Archivo ya existente, si desea sobreescribirlo use la opción '-o'\n");
        	return -1;
            }
        }
        
        crea_sala(capacidad);
        
        if (guarda_estado_sala(ruta) != 0) {
            fprintf(stderr, "Hubo un error al intentar guardar el estado de la sala en el archivo: %s\n", strerror(errno));
            return 1;
        }
 
    /*RESERVAR SALA*/
    } else if (strcmp(orden, "reserva") ==0) {
    
        if (ruta == NULL) {
            fprintf(stderr, "Por favor, introduzca la ruta del archivo.\n");
            return 1;
        }
        
        if(argc<5){
    	    fprintf(stderr, "Uso: %s reserva -f ruta id_persona1 [id_persona2 ...]\n", argv[0]);
    	}
    	
    	recupera_estado_sala(ruta);
	if (capacidad_sala()==-1){
		fprintf(stderr,"Hubo un error, sala no existente.\n");
		return -1;
	}
	
	if (argc - optind > capacidad_sala()) {
		fprintf(stderr,"Hubo un error al intentar reservar el asiento, la sala está llena.\n");
    		return -1;
	}
	
	int fd = open(ruta, O_RDWR);
	if (fd == -1) {
            perror("Hubo un error al intentar abrir el archivo\n");
            return -1;
        }
        
	for (int i = 4; i < argc; i++) {
             int id = atoi(argv[i]);
             int asiento = reserva_asiento(id);
             if (asiento == -1){
 		   fprintf(stderr, "Hubo un error al intentar reservar un asiento\n");
	    	   close(fd);
	    	   return -1;
             }
        }
        
        guarda_estado_sala(ruta);
        estado_sala();
        close(fd);
        return 0;
    	
    /*ANULAR SALA*/
    } else if (strcmp(orden, "anula") == 0) {
        if (ruta==NULL) {
            fprintf(stderr, "Hubo un error, falta la ruta del archivo.\n");
            return 1;
        }
        
        if (argc<6){
    		fprintf(stderr, "Hubo un error, faltan argumentos\n");
    		return -1;
    	}
    	
    	recupera_estado_sala(ruta);
	if (capacidad_sala() == -1) {
		fprintf(stderr, "Sala no existente\n");
	}
	
	int fd = open(ruta, O_RDWR);
        if (fd==-1) {
            perror("Hubo un error al intentar liberar el asiento.\n");
            return -1;
        }
        
        if (indice_a ==-1 || indice_a >= argc - 1) { 
            fprintf(stderr,"Falta el argumento para la opción -a.\n");
            return 1;
        }
        
        if (aorp==1) {
	    for (int i = indice_a + 1; i < argc; i++) {
		int id = atoi(argv[i]);
		int liberar = libera_asiento(id);
		if (liberar == -1) {
		     fprintf(stderr, "Hubo un error al intentar liberar el asiento.\n");
			close(fd);
		     }
		     printf("El asiento %d, ya no está ocupado por el ID %d\n", id, liberar);
		}
	} else if (aorp == 0) {
	    for (int j = indice_a + 1; j < argc; j++) {
		int id = atoi(argv[j]);
		int asientos_liberados = 0;
		for (int i = 0; i < capacidad_sala(); i++) {
		     if (sala[i] == id) {
		         printf("El asiento %d está reservado por el ID %d\n", i + 1, id);
		         asientos_liberados++;
		     }
		}
		if (asientos_liberados == 0) {
		     fprintf(stderr, "Hubo un error, el ID %d no tiene asientos reservados.\n", id);
		} else {
		     int asiento_a_liberar;
		     printf("Por favor, introduzca el número de asiento que desea liberar: ");
		     scanf("%d", &asiento_a_liberar);
		     if (asiento_a_liberar < 1 || asiento_a_liberar > capacidad_sala()) {
		         fprintf(stderr, "Hubo un error, el número de asiento no es válido.\n");
		         close(fd);
		         return -1;
		     } else if (sala[asiento_a_liberar - 1] != id) {
		            fprintf(stderr, "Error: El asiento %d no está ocupado por el ID %d.\n", asiento_a_liberar, id);
		            close(fd);
		            return -1;
		        } else {
		            int libera = libera_asiento(asiento_a_liberar);
		            if (libera == -1) {
		                fprintf(stderr, "Hubo un error al intentar liberar el asiento\n");
		                close(fd);
		                return -1;
		            }
		            printf("El asiento %d ocupado por el ID %d ha sido liberado.\n", asiento_a_liberar, id);
		        }
		    }
		}
		
	}
	
        guarda_estado_sala(ruta);
        estado_sala();
        close(fd);
    	
    /*ESTADO DE LA SALA*/
    } else if (strcmp(orden, "estado") == 0) {
        if (recupera_estado_sala(ruta) != 0) {
            fprintf(stderr, "Hubo un error al intentar recuperar el estado de la sala desde el archivo.\n");
            return 1;
        }
        estado_sala();
        
    /*COMPARAR ESTADO*/
    } else if (strcmp(orden, "compara") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Uso: %s misala <ruta1> <ruta2>.\n", argv[0]);
                return 1;
            }
            
            char* ruta1 = argv[2];
            char* ruta2 = argv[3];
            
            int fd1 = open(ruta1, O_RDONLY);
            int fd2 = open(ruta2, O_RDONLY);
            
            if (fd1 == -1 || fd2 == -1) {
                fprintf(stderr,"Hubo un error al intentar abrir los archivos\n");
                return -1;
            }

            char string1[400];
            char string2[400];
            int lectura1, lectura2;
            
            while ((lectura1 = read(fd1, string1, sizeof(string1))) > 0) {
                lectura2 = read(fd2, string2, sizeof(string2));
                
                if (lectura1 != lectura2 || memcmp(string1, string2, lectura1) != 0) {
                    printf("El archivo %s y %s son diferentes.\n", ruta1, ruta2);
                    close(fd1);
                    close(fd2);
                    return 1;
                }
            }
            if (lectura1 == -1 || lectura2 == -1) {
                fprintf(stderr,"Hubo un error al intentar leer los archivos");
                close(fd1);
                close(fd2);
                return -1;
            }

            printf("El archivo %s y %s son iguales.\n", ruta1, ruta2);
            
            close(fd1);
            close(fd2);
            
            return 0;
            
    } else {
        fprintf(stderr, "Hubo un error, no se ha reconocido la orden.\n");
        return 1;
    }

    return 0;
}

