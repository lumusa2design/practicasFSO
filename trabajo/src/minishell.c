#include "sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#define MAX_INPUT_LENGTH 100

void reserva_term(int id)
{
    bool ocupado = false;
    for (int i = 0; i <= capacidad_sala(); i++) {
        if (estado_asiento(i) == id) {
            ocupado = true;
            break; 
        }
    }

    if (!ocupado) {
        reserva_asiento(id);
        printf("Se ha reservado un asiento para el ID: %d\n", id);
    } else {
        printf("Ya existe un asiento reservado para el ID: %d. Por favor, introduzca otro ID\n", id);
    }
}


void libera_term(int id) 
{
    bool ocupado;

    for (int i = 0; i <= capacidad_sala(); i++) {
        if (estado_asiento(i) == id) {
            libera_asiento(i);
            ocupado = true;
            printf("Se ha liberado el asiento con ID: %d\n", id);
            break; 
        }
    }
    if (!ocupado) {
        printf("No existe ninguna reserva con el ID: %d\n", id);
    }
}

void estado_asiento_term(int id_asiento) {
    int estado = estado_asiento(id_asiento);
    
    if (estado > 0) {
        printf("El asiento está reservado con el ID: %d\n", estado);
    } else if (estado == 0) {
        printf("El asiento está libre\n");
    } else {
        printf("Por favor, introduzca un valor entre 0 y %d\n", capacidad_sala() - 1);
    }
}


void estado_sala_term() {
    printf("Capacidad de la sala: %d\n", capacidad_sala());
    printf("Asientos ocupados: %d\n", asientos_ocupados());
    printf("Asientos libres: %d\n", asientos_libres());
    imprimir();
}

void cerrar_sala_term() {
    int val;
    if (asientos_libres() == capacidad_sala()) {
        printf("\nLa sala se cerró vacía:\n");
        val = 0;
    } else {
        printf("\nLa sala se cerró con público:\n");
            printf("Capacidad de la sala: %d\n", capacidad_sala());
    printf("Asientos ocupados: %d\n", asientos_ocupados());
    printf("Asientos libres: %d\n", asientos_libres());
        val = 1;
    }
    estado_sala_term();
    elimina_sala();
    exit(val);
}



void help() {

    printf("\n_______________________________LISTA DE COMANDOS:_______________________________\n\n\n");
    printf("  reserva <id persona>        : Se reserva un asiento para el ID pasado\n");
    printf("  liberar <id persona>        : Se libera el asiento ocupado con el ID pasado\n");
    printf("  estado_asiento <id asiento> : Se obtiene el estado del asiento con el ID pasado\n");
    printf("  estado_sala                 : Se muestra el estado de la sala\n");
    printf("  cerrar_sala                 : Se cierra la sala.\n");
    printf("___________________________________________________________________________________\n");
}


void parse_command(char *command, char *action, int *argument) {
    char *token = strtok(command, " ");

    if (token != NULL) {
        strcpy(action, token);
        token = strtok(NULL, " ");
        *argument = (token != NULL) ? atoi(token) : -1;
    } else {
        strcpy(action, "");
        *argument = -1;
    }
}


void hangler_signal_child(int sig) {
    if (sig == SIGUSR1) {
        printf("Señal recibida. Finalizando proceso hijo...\n");
        cerrar_sala_term();
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("\nIntroduce: %s + 'capacidad'\n", argv[0]);
        return 1;
    }

    int capacidad = atoi(argv[1]);
    crea_sala(capacidad);
    signal(SIGUSR1, hangler_signal_child);
    char input[MAX_INPUT_LENGTH];
    char action[MAX_INPUT_LENGTH];
    int argument;

    while (1) {
        printf("\n\nESCOGE UNA OPCIÓN: \n\n**(Para ver la lista de comandos escribe 'commands') ");
        fgets(input, MAX_INPUT_LENGTH, stdin);
        input[strcspn(input, "\n")] = '\0'; 
        
        parse_command(input, action, &argument);

        if (strcmp(action, "commands") == 0) {
            help();
        } else if (strcmp(action, "reserva") == 0) {
            if (argument != -1) {
                reserva_term(argument);
            } else {
                printf("Introduzca el ID de la persona.\n");
            }
        } else if (strcmp(action, "libera") == 0) {
            if (argument != -1) {
                libera_term(argument);
            } else {
                printf("Introduzca el ID de la persona.\n");
            }
        } else if (strcmp(action, "estado_asiento") == 0) {
            if (argument != -1) {
                estado_asiento_term(argument);
            } else {
                printf("Introduzca el ID de la persona.\n");
            }
        } else if (strcmp(action, "estado_sala") == 0) {
            estado_sala_term();
        } else if (strcmp(action, "cerrar_sala") == 0) {
            cerrar_sala_term();
        } else {
            printf("Comando desconocido. Introduzca un comando válido o 'commands' para ver la lista de comandos.\n");
        }
    }
    
    return 0;
}

