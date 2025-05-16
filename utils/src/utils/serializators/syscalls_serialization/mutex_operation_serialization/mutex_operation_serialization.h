#ifndef UTILS_MUTEX_OPERATION_SERIALIZATION_H_
#define UTILS_MUTEX_OPERATION_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

//Para la creacion de semaforos, se ignoraria el campo tid
typedef struct {
    char* nombre_semaforo;
    uint32_t tamanio_nombre;
    uint32_t tid;
} t_mutex_operation;

t_mutex_operation* create_syscall_mutex_operation(char* nombre_semaforo, uint32_t tid);

void serialize_mutex_operation(t_mutex_operation* mutex_operation, t_buffer* buffer);

t_mutex_operation* deserialize_mutex_operation(void* stream);

void destroy_mutex_operation(t_mutex_operation* mutex_operation);

#endif