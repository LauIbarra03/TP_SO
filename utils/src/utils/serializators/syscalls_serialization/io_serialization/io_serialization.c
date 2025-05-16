#include "io_serialization.h"

t_syscall_io* create_syscall_io(uint32_t milisegundos) {
    t_syscall_io* syscall_io = malloc(sizeof(t_syscall_io));
    syscall_io->milisegundos = milisegundos;

    return syscall_io;
}

void serialize_syscall_io(t_syscall_io* syscall_io, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total del stream
    size_t size = sizeof(uint32_t);
    buffer->size = size;

    buffer->stream = malloc(size);

    // Copiar el campo al stream
    memcpy(buffer->stream + buffer->offset, &(syscall_io->milisegundos), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_syscall_io* deserialize_syscall_io(void* stream) {
    t_syscall_io* syscall_io = malloc(sizeof(t_syscall_io));
    int offset = 0;

    // Leer los milisegundos
    memcpy(&(syscall_io->milisegundos), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return syscall_io;
}

void destroy_syscall_io(t_syscall_io* syscall_io) {
    if (syscall_io != NULL) {
        free(syscall_io); // Liberar la estructura
    }
}
