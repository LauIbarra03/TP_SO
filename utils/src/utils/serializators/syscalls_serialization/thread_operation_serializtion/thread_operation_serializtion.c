#include "thread_operation_serializtion.h"

t_thread_operation* create_syscall_thread_operation(uint32_t tid) {
    t_thread_operation* hilo_a_operar = malloc(sizeof(t_thread_operation));
    hilo_a_operar -> tid = tid;

    return hilo_a_operar;
}


void serialize_thread_operation(t_thread_operation* thread_operation, t_buffer* buffer) {
    buffer -> offset = 0;

    size_t size = sizeof(u_int32_t);
    buffer -> size = size;

    buffer->stream = malloc(size);
    memcpy(buffer -> stream + buffer -> offset, &(thread_operation -> tid), sizeof(u_int32_t));
}


t_thread_operation* deserialize_thread_operation(void* stream) {
    t_thread_operation* thread = malloc(sizeof(t_thread_operation));
    int offset = 0;

    memcpy(&(thread->tid), stream + offset, sizeof(uint32_t));

    return thread;
}

void destroy_thread_operation(t_thread_operation* thread_operation) {
    if (thread_operation != NULL) {
        free(thread_operation);
    }
}