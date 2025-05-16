#ifndef UTILS_EXECUTION_CONTEXT_SERIALIZATION_H_
#define UTILS_EXECUTION_CONTEXT_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>



typedef struct {
    u_int32_t tid;
    u_int32_t pid;
    t_state estado;
    t_register *registers;
    uint32_t tamanio_proceso;
} t_execution_context;

typedef struct{
    uint32_t pid;
    uint32_t tid;
    uint32_t tamanio_proceso;
    void* datos;
} t_dump_context;



void serialize_execution_context(t_execution_context* execution_context, t_buffer* buffer);

t_execution_context* deserialize_execution_context(void* stream);

t_execution_context* create_execution_context(u_int32_t tid, u_int32_t pid, t_register* registers, t_state estado, uint32_t tamanio_proceso);

void destroy_execution_context(t_execution_context* execution_context);

void log_execution_context(t_execution_context* execution_context, char* type);

// ======  DUMP CONTEXT  ======

void serialize_dump_context(t_dump_context* dump_context, t_buffer* buffer);

t_dump_context* create_dump_context(uint32_t pid, uint32_t tid, uint32_t size, void* datos);

t_dump_context* deserialize_dump_context(void* stream);

void destroy_dump_context(t_dump_context* dump_context);

#endif

