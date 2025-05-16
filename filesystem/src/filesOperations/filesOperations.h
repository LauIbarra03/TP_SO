#include "filesystem.h"
#include <bitmapOperations/bitmapOperations.h>

// Crear archivo en el sistema
int crear_archivo(const char* nombre_archivo, uint32_t tamaño_inicial, void* datos);

int existe_archivo(const char* nombre_archivo);