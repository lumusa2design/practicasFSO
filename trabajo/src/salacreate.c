void create_file(char *ruta) {
    int fd = open(ruta, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        print_error("Ruta no válida, error al crear el archivo.\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int create_operators(int argc, char* argv[], int *capacidad, char ** ruta, int *sobreescribir){
    int permisos = -1;
    
    int opt;
    while ((opt = getopt(argc, argv, "f::c:")) != -1) {
        switch (opt) {
            case 'f':
                if (optarg != NULL && strcmp(optarg, "o") == 0) {
                    if (optind < argc && argv[optind][0] != '-') {
                        *ruta = argv[optind];
                        permisos = check_ruta(*ruta);
                        optind++;
                        *sobreescribir = 1;
                    } else {
                        print_error("Falta la ruta del archivo después de -fo\n");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    *ruta = argv[optind];
                    permisos = check_ruta(*ruta);
                    if (permisos >= 0) {
                        print_error("El archivo ya existe (Use el argumento -fo para sobreescribir)\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
                
            case 'c':
                *capacidad = atoi(optarg);
                break;
                
            default:
                print_error("Uso: ./misala -f[o] <ruta_archivo> -c <capacidad>\n");
                exit(EXIT_FAILURE);
        }
    }
    
    return permisos;
}

int crear_misala(int argc, char* argv[]){
    int capacidad = 0;
    char* ruta = NULL;
    int sobreescribir = 0;

    int permisos = create_operators(argc, argv, &capacidad, &ruta, &sobreescribir);

    if (argc != 6) {
        print_error("Error en el comando\n");
        return -1;
    }
      
    if (permisos == -1 && sobreescribir == 0) {
        if (crea_sala(capacidad) == -1) {
            print_error("Hubo un error al crear la sala");
            exit(EXIT_FAILURE);
        }
        create_file(ruta);
        if(guarda_estado_sala(ruta)==-1){
            print_error("Hubo un error al guardar la sala");
            exit(EXIT_FAILURE);
        };
        elimina_sala();
        return 0;
    } else if (permisos > 0 && sobreescribir == 1 ){
        if(permisos == 2 || permisos == 6){
            if (crea_sala(capacidad) == -1) {
                print_error("Hubo un error al crear la sala");
                exit(EXIT_FAILURE);
            }

            if(guarda_estado_sala(ruta)==-1){
                print_error("Hubo un error al guardar la sala");
                exit(EXIT_FAILURE);
            };
            elimina_sala();
            return 0;	
        } else {
            print_error("No posee permiso de escritura en la ruta indicada");
            exit(EXIT_FAILURE);
            return -1;
        }
    } else {
        print_error("Ruta no válida");
        exit(EXIT_FAILURE);
        return -1;
    }
}

