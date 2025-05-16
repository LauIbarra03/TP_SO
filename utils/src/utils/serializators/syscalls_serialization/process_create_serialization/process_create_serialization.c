#include "process_create_serialization.h"
t_process_create* create_syscall_process_create(char* nombre_archivo, uint32_t tamanio_proceso, uint32_t prioridad) {
    t_process_create* nuevo_proceso = malloc(sizeof(t_process_create));
    nuevo_proceso->nombre_archivo = strdup(nombre_archivo); // Copiar el string dinámicamente
    nuevo_proceso->tamanio_nombre = strlen(nombre_archivo) + 1; // Incluir espacio para el null terminator
    nuevo_proceso->tamanio_proceso = tamanio_proceso;
    nuevo_proceso->prioridad = prioridad;

    return nuevo_proceso;
}

void serialize_process_create(t_process_create* process_create, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total del stream
    size_t size = sizeof(uint32_t) + process_create->tamanio_nombre + sizeof(uint32_t) + sizeof(uint32_t);
    buffer->size = size;

    buffer->stream = malloc(size);

    // Copiar cada campo al stream
    memcpy(buffer->stream + buffer->offset, &(process_create->tamanio_nombre), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, process_create->nombre_archivo, process_create->tamanio_nombre);
    buffer->offset += process_create->tamanio_nombre;

    memcpy(buffer->stream + buffer->offset, &(process_create->tamanio_proceso), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, &(process_create->prioridad), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_process_create* deserialize_process_create(void* stream) {
    t_process_create* process_create = malloc(sizeof(t_process_create));
    int offset = 0;

    // Leer el tamaño del nombre
    memcpy(&(process_create->tamanio_nombre), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer el nombre del archivo
    process_create->nombre_archivo = malloc(process_create->tamanio_nombre);
    memcpy(process_create->nombre_archivo, stream + offset, process_create->tamanio_nombre);
    offset += process_create->tamanio_nombre;

    // Leer el tamaño del proceso
    memcpy(&(process_create->tamanio_proceso), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer la prioridad
    memcpy(&(process_create->prioridad), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return process_create;
}

void destroy_process_create(t_process_create* process_create) {
    if (process_create != NULL) {
        if (process_create->nombre_archivo != NULL) {
            free(process_create->nombre_archivo); // Liberar el nombre dinámico
        }
        free(process_create); // Liberar la estructura
    }
}
