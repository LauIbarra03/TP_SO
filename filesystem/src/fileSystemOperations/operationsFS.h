#ifndef UTILSFS_H
#define UTILSFS_H

// Cargar el bitmap en memoria
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "filesystem.h"
#include <utils/serializators/execution_context_serialization/execution_context_serialization.h>
#include <bitmapOperations/bitmapOperations.h>
#include <filesOperations/filesOperations.h>

// Estructura para representar un archivo en el FS

extern uint32_t block_size;
extern uint32_t block_count;
extern const char* mount_dir;
extern size_t bitmap_size;
extern pthread_mutex_t bitmap_mutex;
extern pthread_mutex_t file_bitmap_mutex;
extern pthread_mutex_t file_bloques_mutex;
extern char bitmap_path[256], blocks_path[256], files_dir[256];

typedef struct {
    char* nombre;       // Nombre del archivo
    uint32_t size;           // Tamaño del archivo en bytes
    uint32_t index_block;    // Bloque índice asignado al archivo
} FileMetadata;

void inicializar_fs();

//Crear directorio completo (chequea si existe antes)
int crear_directorio_recursivo(const char* ruta);

// Crear archivo si no existe y llenarlo con un valor inicial
void crear_archivo_si_no_existe(const char* path, size_t size, char fill_byte) ;

op_code execute_dump_memory(t_dump_context* contexto);

void generar_nombre_archivo(uint32_t pid, uint32_t tid, char nombre[256]);

#endif