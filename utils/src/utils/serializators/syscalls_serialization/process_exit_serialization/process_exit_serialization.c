#include "process_exit_serialization.h"

t_process_exit* create_syscall_process_exit(uint32_t pid) {
    t_process_exit* proceso_exit = malloc(sizeof(t_process_exit));
    proceso_exit -> pid = pid;

    return proceso_exit;
}

void serialize_process_exit(t_process_exit* process_exit, t_buffer* buffer) {
    buffer -> offset = 0;

    size_t size = sizeof(u_int32_t);
    buffer -> size = size;

    buffer->stream = malloc(size);

    memcpy(buffer->stream + buffer->offset, &(process_exit -> pid), sizeof(uint32_t));
}

t_process_exit* deserialize_process_exit(void* stream){
    
    t_process_exit* process_exit = malloc(sizeof(t_process_exit));
    int offset = 0;

    memcpy(&(process_exit->pid), stream + offset, sizeof(uint32_t));

    return process_exit;
}

void destroy_process_exit(t_process_exit* process_exit) {
    if (process_exit != NULL) {
        free(process_exit);
    }    
}
