#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_LENGTH 100
#define MAX_SALAS 100

struct dict_salas 
{
    char name[MAX_INPUT_LENGTH];
    pid_t pid;
};

struct dict_salas total_salas[MAX_SALAS];

int salas_ocupadas;

int search_pid(pid_t pid) {
    for (int i = 0; i < salas_ocupadas; i++) {
        if (total_salas[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

char* search_by_name_sala_pid(pid_t pid) {
    int index = search_pid(pid);
    if (index != -1) {
        return total_salas[index].name;
    }
    return NULL;
}

void delete_sala_pid(pid_t pid) {
    int index = search_pid(pid);
    if (index != -1) {
        for (int i = index; i < salas_ocupadas - 1; i++) {
            total_salas[i] = total_salas[i + 1];
        }
        salas_ocupadas--;
    }
}

void sigchld_handler(int signal) {
    int state;
    pid_t pid;

    while ((pid = wait(&state)) > 0) {
        if (WIFEXITED(state)) {
            int exit_state = WEXITSTATUS(state);
            char *name_sala = search_by_name_sala_pid(pid);
            if (name_sala != NULL) {
                delete_sala_pid(pid);
                if (exit_state == 0) {
                    printf("\nLa sala '%s' ha cerrado con el aforo completo\n", name_sala);
                } else {
                    printf("\nLa sala '%s' ha cerrado, con un total de %d asientos disponibles\n", name_sala, exit_state);
                }
                printf("Ingrese el nombre de la ciudad y la capacidad (o teclee salir para terminar)\n");
            }
        }
    }
}

void crea_sucursal(const char* ciudad, int capacidad) {
    char capacity_str[10];
    snprintf(capacity_str, sizeof(capacity_str), "%d", capacidad);

    pid_t pid = fork();
    if (pid == 0) {
        execl("/usr/bin/xterm", "xterm", "-T", ciudad, "-e", "./minishell", capacity_str, NULL);
        perror("Execl Failure");
        _exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Fork Failure");
    } else {
        strcpy(total_salas[salas_ocupadas].name, ciudad);
        total_salas[salas_ocupadas].pid = pid;
        salas_ocupadas++;
    }
}

void signal_child() {
    for (int i = 0; i < salas_ocupadas; i++) {
        printf("Sala: %s, PID: %d\n", total_salas[i].name, total_salas[i].pid);
        kill(total_salas[i].pid, SIGUSR1);
        waitpid(total_salas[i].pid, NULL, 0);
    }
}

int main() {
    struct sigaction action;

    action.sa_handler = sigchld_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &action, NULL) == -1) {
        perror("Failure in SIGCHLD Handler");
        exit(EXIT_FAILURE);
    }

    char name_sala[MAX_INPUT_LENGTH];
    int capacity = 10;

    while (1) {
        printf("\nIntroduzca el nombre y la capacidad de la sala (o teclee 'salir' para terminar):\n");
        scanf("%s", name_sala);
        if (strcmp("salir", name_sala) == 0) {
            break;
        }
        scanf("%d", &capacity);
        crea_sucursal(name_sala, capacity);
    }
    signal_child();

    return 0;
}

