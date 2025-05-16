#ifndef UTILS_PROCESS_CREATE_SERIALIZATION_H_
#define UTILS_PROCESS_CREATE_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    char* nombre_archivo;
    uint32_t tamanio_nombre;
    u_int32_t tamanio_proceso;
    uint32_t prioridad;
} t_process_create;

t_process_create* create_syscall_process_create(char* nombre_archivo, uint32_t tamanio_proceso, uint32_t prioridad);

void serialize_process_create(t_process_create* process_create, t_buffer* buffer);

t_process_create* deserialize_process_create(void* stream);

void destroy_process_create(t_process_create* process_create);
#endif