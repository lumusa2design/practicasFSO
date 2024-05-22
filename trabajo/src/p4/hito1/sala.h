#include <stddef.h>

int reserva_asiento (int id_persona);
int libera_asiento (int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres();
int asientos_ocupados();
int capacidad_sala(); 
int crea_sala(int capacidad);
void estado_sala();
int elimina_sala();
int guarda_estado_sala(const char* ruta_fichero);
int recupera_estado_sala(const char* ruta_fichero);
int guarda_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);
int recupera_estadoparcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);

extern int* sala;
