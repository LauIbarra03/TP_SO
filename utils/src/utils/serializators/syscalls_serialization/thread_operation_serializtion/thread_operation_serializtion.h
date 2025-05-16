
#ifndef UTILS_THREAD_OPERATION_SERIALIZATION_H_
#define UTILS_THREAD_OPERATION_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t tid ;   
} t_thread_operation;

t_thread_operation* create_syscall_thread_operation(uint32_t tid);

void serialize_thread_operation(t_thread_operation* thread_operation, t_buffer* buffer);

t_thread_operation* deserialize_thread_operation(void* stream);

void destroy_thread_operation(t_thread_operation* thread_operation);

#endif

