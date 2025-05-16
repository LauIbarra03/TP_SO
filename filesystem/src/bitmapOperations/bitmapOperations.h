#ifndef BITMAPOPERATIONS 
#define BITMAPOPERATIONS 

#include "filesystem.h"
#include <filesOperations/filesOperations.h>

void cargar_bitmap();
void persistir_bitmap();
int buscar_bloque_libre(uint32_t* byte_index, uint32_t* bit_position);
void marcar_bloque(uint32_t bloque, int ocupado);
int asignar_bloques(uint32_t size, const char* blocks_path, const char* new_file, void* datos, int* bloque_indice);
size_t contar_bloques_libres(); 

#endif