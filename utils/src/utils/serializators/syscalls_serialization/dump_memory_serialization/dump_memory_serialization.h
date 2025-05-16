#ifndef UTILS_DUMP_MEMORY_SERIALIZATION_H_
#define UTILS_DUMP_MEMORY_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t pid;
    uint32_t tid;
} t_dump_memory;


t_dump_memory* create_dump_memory(uint32_t pid, uint32_t tid);

void serialize_dump_memory(t_dump_memory* dump_memory, t_buffer* buffer);

t_dump_memory* deserialize_dump_memory(void* stream);

void destroy_dump_memory(t_dump_memory* dump_memory) ;

#endif