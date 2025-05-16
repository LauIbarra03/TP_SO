#ifndef UTILS_PROCESS_EXIT_SERIALIZATION_H_
#define UTILS_PROCESS_EXIT_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t pid;
} t_process_exit;

t_process_exit* create_syscall_process_exit(uint32_t pid);

void serialize_process_exit(t_process_exit* process_exit, t_buffer* buffer);

t_process_exit* deserialize_process_exit(void* stream);

void destroy_process_exit(t_process_exit* process_exit);

#endif