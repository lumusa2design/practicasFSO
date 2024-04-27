#ifndef SALA_H
#define SALA_H
#include <stddef.h>

void imprimir();
int crea_sala (int capacidad);
int reserva_asiento (int id_persona);
int libera_asiento (int id_asiento);
int estado_asiento(int id_asiento);

int asientos_libres ();
int asientos_ocupados ();
int capacidad_sala ();
int elimina_sala();

int guarda_estado_sala(char* ruta_fichero);
int recupera_estado_sala(char* ruta_fichero);
int guarda_estadoparcial_sala( char* ruta_fichero, size_t num_asientos, int* id_asientos);
int recupera_estadoparcial_sala( char* ruta_fichero, size_t num_asientos, int* id_asientos);

#endif