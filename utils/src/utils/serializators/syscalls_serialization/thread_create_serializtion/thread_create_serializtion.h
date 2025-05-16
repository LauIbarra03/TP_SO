
#ifndef UTILS_THREAD_CREATE_SERIALIZATION_H_
#define UTILS_THREAD_CREATE_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    char* nombre_archivo;
    uint32_t tamanio_nombre;
    uint32_t prioridad;
} t_thread_create;

t_thread_create* create_syscall_thread_create(char* nombre_archivo, uint32_t prioridad);

void serialize_thread_create(t_thread_create* thread_create, t_buffer* buffer);

t_thread_create* deserialize_thread_create(void* stream);

void destroy_thread_create(t_thread_create* thread_create);
#endif