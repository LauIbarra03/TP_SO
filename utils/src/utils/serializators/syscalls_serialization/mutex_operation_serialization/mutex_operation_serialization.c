#include "mutex_operation_serialization.h"

t_mutex_operation* create_syscall_mutex_operation(char* nombre_semaforo, uint32_t tid) {
    t_mutex_operation* nueva_operacion = malloc(sizeof(t_mutex_operation));
    nueva_operacion->nombre_semaforo = strdup(nombre_semaforo); // Copiar dinámicamente el nombre del semáforo
    nueva_operacion->tamanio_nombre = strlen(nombre_semaforo) + 1; // Incluir espacio para el null terminator
    nueva_operacion->tid = tid;

    return nueva_operacion;
}

void serialize_mutex_operation(t_mutex_operation* mutex_operation, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total del stream
    size_t size = sizeof(uint32_t) + mutex_operation->tamanio_nombre + sizeof(uint32_t);
    buffer->size = size;

    buffer->stream = malloc(size);

    // Copiar cada campo al stream
    memcpy(buffer->stream + buffer->offset, &(mutex_operation->tamanio_nombre), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, mutex_operation->nombre_semaforo, mutex_operation->tamanio_nombre);
    buffer->offset += mutex_operation->tamanio_nombre;

    memcpy(buffer->stream + buffer->offset, &(mutex_operation->tid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_mutex_operation* deserialize_mutex_operation(void* stream) {
    t_mutex_operation* mutex_operation = malloc(sizeof(t_mutex_operation));
    int offset = 0;

    // Leer el tamaño del nombre
    memcpy(&(mutex_operation->tamanio_nombre), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer el nombre del semáforo
    mutex_operation->nombre_semaforo = malloc(mutex_operation->tamanio_nombre);
    memcpy(mutex_operation->nombre_semaforo, stream + offset, mutex_operation->tamanio_nombre);
    offset += mutex_operation->tamanio_nombre;

    // Leer el TID
    memcpy(&(mutex_operation->tid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return mutex_operation;
}

void destroy_mutex_operation(t_mutex_operation* mutex_operation) {
    if (mutex_operation != NULL) {
        if (mutex_operation->nombre_semaforo != NULL) {
            free(mutex_operation->nombre_semaforo); // Liberar el nombre dinámico
        }
        free(mutex_operation); // Liberar la estructura
    }
}
    

