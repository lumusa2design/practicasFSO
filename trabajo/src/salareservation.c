int reservation_operators(int argc, char* argv[], char **ruta, int **id_personas, int *num_personas) {
    int opt;
    int aux = 1;
    // Parsear los argumentos de la línea de comandos
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                *ruta = optarg;
                break;
            default:
                print_error("Uso: ./misala reserva -f <ruta_archivo> <id_persona1> <id_persona2> ...\n");
                exit(EXIT_FAILURE);
                return -1;
        }
    }
    
    if (argc < 5) {
        print_error("Error en el comando\n");
        exit(EXIT_FAILURE);
        return -1;
    }
    
    // Calcular el número de personas
    *num_personas = argc - optind - aux;
    
    // Asignar memoria para id_personas
    *id_personas = malloc(*num_personas * sizeof(int));
    if (*id_personas == NULL) {
        print_error("No se pudo asignar memoria para almacenar los IDs de las personas.\n");
        exit(EXIT_FAILURE);
        return -1;
    }
    
    // Extraer los IDs de las personas
    for (int pos = 0; pos < *num_personas; pos++) {
        int verificar_num = atoi(argv[++optind]); 
        if (verificar_num != 0 && verificar_num > 0) {
            (*id_personas)[pos] = verificar_num;
        } else {
            free(*id_personas);
            *id_personas = NULL;
            fprintf(stderr, "ERROR CONTROLADO: %s no es el id de un asiento\n", argv[optind]);
            exit(EXIT_FAILURE);
            return -1;
        }
    }
    
    return 0;
}

int reserva_misala(int argc, char* argv[]) {
    char* ruta = NULL;
    int* id_personas = NULL;
    int num_personas = 0;

    if (reservation_operators(argc, argv, &ruta, &id_personas, &num_personas) != 0) {
        return -1;
    }

    int permisos = check_ruta(ruta);
    if (permisos == -1) {
        print_error("Ruta no válida\n");
        exit(EXIT_FAILURE);
        return -1;
    } else if (permisos != 6) { 
        print_error("Debe tener permiso de lectura y escritura en la ruta indicada\n");
        exit(EXIT_FAILURE);
        return -1;
    }
    
    // Verificar si hay una sala creada
    int capacidad_sala = comprueba_si_fichero_contiene_info_sala(ruta);
    if (capacidad_sala != -1) {
        crea_sala(capacidad_sala);
        recupera_estado_sala(ruta);
        
        if (asientos_libres() >= num_personas) {
            int *id_asientos = malloc(num_personas * sizeof(int));
            
            // Verificar si la asignación fue exitosa
            if (id_asientos == NULL) {
                // Manejar el error
                print_error("No se pudo asignar memoria para el array.\n");
                elimina_sala();
                free(id_personas);
                free(id_asientos);
                id_personas = NULL;
                id_asientos = NULL;
                exit(EXIT_FAILURE);
                return -1;
            } else {
                for(int reserva = 0; reserva < num_personas; reserva++) {
                    id_asientos[reserva] = reserva_asiento(id_personas[reserva]);
                }
                guarda_estado_parcial_sala(ruta, num_personas, id_asientos);
                elimina_sala();
                free(id_personas);
                free(id_asientos);
                id_personas = NULL;
                id_asientos = NULL;
                return 0;
            }
        } else {
            print_error("La sala no tiene suficientes asientos libres para todas las personas\n");
            elimina_sala();
            free(id_personas);
            id_personas = NULL;
            exit(EXIT_FAILURE);
            return -1;
        }
    } else {
        print_error("El archivo no guarda la información de una sala\n");
        elimina_sala();
        free(id_personas);
        exit(EXIT_FAILURE);
        return -1;
    }
}

