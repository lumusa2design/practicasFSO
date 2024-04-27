void state_operators(int argc, char* argv[], char **ruta) {
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                if(optarg == NULL) {
                    print_error("Error en el comando\n");
                    exit(EXIT_FAILURE);
                }
                *ruta = optarg;
                break;
            default:
                print_error("Uso: ./misala estado -f <archivo>\n");
                exit(EXIT_FAILURE);
        }
    }
    
    if (*ruta == NULL || argc != 4) {
        print_error("Error en el comando\n");
        exit(EXIT_FAILURE);
    }
}

void check_root_permissions(char *ruta) {
    int permisos = check_ruta(ruta);
    if (permisos == -1) {
        print_error("Ruta no válida\n");
        exit(EXIT_FAILURE);
    } else if (permisos < 4) {
        print_error("No se posee permiso de lectura en la ruta indicada\n");
        exit(EXIT_FAILURE);
    }
}

int execute_state(const char *ruta) {
    int capacidad_sala = comprueba_si_fichero_contiene_info_sala(ruta);
    if (capacidad_sala != -1) {
        crea_sala(capacidad_sala);
        recupera_estado_sala(ruta);
        print_asientos();
        elimina_sala();
        return 0; // Éxito
    } else {
        print_error("El archivo no guarda la información de una sala\n");
        exit(EXIT_FAILURE);
    }
}

int state_execute(int argc, char* argv[]) {
    char *ruta = NULL;
    state_operators(argc, argv, &ruta);
    check_root_permissions(ruta);
    return execute_state(ruta);
}

