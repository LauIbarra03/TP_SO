#ifndef UTILS_READ_MEM_REQUEST_SERIALIZATION_H_
#define UTILS_READ_MEM_REQUEST_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t physical_address;
    uint32_t pid;
    uint32_t tid;
} t_read_mem_request;


void serialize_read_mem_request(t_read_mem_request* read_mem_request, t_buffer* buffer);

t_read_mem_request*  deserialize_read_mem_request(void* stream);

t_read_mem_request*  create_read_mem_request(u_int32_t tid, u_int32_t pid, uint32_t physical_address);

void destroy_read_mem_request(t_read_mem_request* read_mem_request);

#endif

