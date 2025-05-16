#include "dump_memory_serialization.h"

t_dump_memory* create_dump_memory(uint32_t pid, uint32_t tid) {
    t_dump_memory* dump_memory = malloc(sizeof(t_dump_memory));
    dump_memory->pid = pid;
    dump_memory->tid = tid;

    return dump_memory;
}

void serialize_dump_memory(t_dump_memory* dump_memory, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total del stream
    size_t size = sizeof(uint32_t) + sizeof(uint32_t);
    buffer->size = size;

    buffer->stream = malloc(size);

    // Copiar cada campo al stream
    memcpy(buffer->stream + buffer->offset, &(dump_memory->pid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, &(dump_memory->tid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_dump_memory* deserialize_dump_memory(void* stream) {
    t_dump_memory* dump_memory = malloc(sizeof(t_dump_memory));
    int offset = 0;

    // Leer el PID
    memcpy(&(dump_memory->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer el TID
    memcpy(&(dump_memory->tid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return dump_memory;
}

void destroy_dump_memory(t_dump_memory* dump_memory) {
    if (dump_memory != NULL) {
        free(dump_memory); // Liberar la estructura
    }
}

