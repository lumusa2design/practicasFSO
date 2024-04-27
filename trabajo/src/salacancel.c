int anula_misala(int argc, char* argv[]) {
    int opt;
    char *ruta = NULL;
    int num_asientos = 0;
    int num_personas = 0;
    int tipo_anulacion = 0; // 0: asientos, 1: personas

    // Uso de getopt para analizar las opciones de línea de comandos
    while ((opt = getopt(argc, argv, "f:a:p:")) != -1) {
        switch (opt) {
            case 'f':
                ruta = optarg;
                break;
            case 'a':
                if (strcmp(optarg, "sientos") == 0) {
                    tipo_anulacion = 0;
                } else {
                    fprintf(stderr, "ERROR CONTROLADO: Opción '-a%s' no reconocida\n", optarg);
                    return -1;
                }
                break;
            case 'p':
                if (strcmp(optarg, "ersonas") == 0) {
                    tipo_anulacion = 1;
                } else {
                    fprintf(stderr, "ERROR CONTROLADO: Opción '-p%s' no reconocida\n", optarg);
                    return -1;
                }
                break;
            default:
                print_error("Uso: ./misala -f ruta_fichero -asientos id_asiento1 [id_asiento2...]\n");
                print_error("Uso: ./misala -f ruta_fichero -personas id_persona1 [id_persona2...]\n");
                return -1;
        }
    }

    //Comprobar argumentos del comando
    if (argc < 6) {
        print_error("Error en el comando\n");
        return -1;
    }

    if (tipo_anulacion == 0) {
        num_asientos = argc - optind;
        return anula_misala_asientos(argc, argv, ruta, num_asientos, optind);
    } else if (tipo_anulacion == 1) {
        num_personas = argc - optind;
        return anula_misala_ids(argc, argv, ruta, num_personas, optind);
    } else {
        fprintf(stderr, "ERROR CONTROLADO: Opción '-a' o '-p' no especificada\n");
        return -1;
    }
}

