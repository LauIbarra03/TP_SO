#ifndef UTILS_IO_SERIALIZATION_H_
#define UTILS_IO_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t milisegundos;
} t_syscall_io;

t_syscall_io* create_syscall_io(uint32_t milisegundos);

void serialize_syscall_io(t_syscall_io* syscall_io, t_buffer* buffer);

t_syscall_io* deserialize_syscall_io(void* stream);

void destroy_syscall_io(t_syscall_io* syscall_io);

#endif