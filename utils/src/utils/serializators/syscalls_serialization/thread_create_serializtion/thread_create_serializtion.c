#include "thread_create_serializtion.h"

t_thread_create* create_syscall_thread_create(char* nombre_archivo, uint32_t prioridad) {
    t_thread_create* nuevo_hilo = malloc(sizeof(t_thread_create));
    nuevo_hilo->nombre_archivo = strdup(nombre_archivo); // Copiar el string dinámicamente
    nuevo_hilo->tamanio_nombre = strlen(nombre_archivo) + 1; // Incluir espacio para el null terminator
    nuevo_hilo->prioridad = prioridad;

    return nuevo_hilo;
}

void serialize_thread_create(t_thread_create* thread_create, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total del stream
    size_t size = sizeof(uint32_t) + thread_create->tamanio_nombre + sizeof(uint32_t);
    buffer->size = size;

    buffer->stream = malloc(size);

    // Copiar cada campo al stream
    memcpy(buffer->stream + buffer->offset, &(thread_create->tamanio_nombre), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, thread_create->nombre_archivo, thread_create->tamanio_nombre);
    buffer->offset += thread_create->tamanio_nombre;

    memcpy(buffer->stream + buffer->offset, &(thread_create->prioridad), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_thread_create* deserialize_thread_create(void* stream) {
    t_thread_create* thread_create = malloc(sizeof(t_thread_create));
    int offset = 0;

    // Leer el tamaño del nombre
    memcpy(&(thread_create->tamanio_nombre), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer el nombre del archivo
    thread_create->nombre_archivo = malloc(thread_create->tamanio_nombre);
    memcpy(thread_create->nombre_archivo, stream + offset, thread_create->tamanio_nombre);
    offset += thread_create->tamanio_nombre;

    // Leer la prioridad
    memcpy(&(thread_create->prioridad), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return thread_create;
}

void destroy_thread_create(t_thread_create* thread_create) {
    if (thread_create != NULL) {
        if (thread_create->nombre_archivo != NULL) {
            free(thread_create->nombre_archivo); // Liberar el nombre dinámico
        }
        free(thread_create); // Liberar la estructura
    }
}
