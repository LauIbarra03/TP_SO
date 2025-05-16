#ifndef UTILS_WRITE_MEM_REQUEST_SERIALIZATION_H_
#define UTILS_WRITE_MEM_REQUEST_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t physical_address;
    uint32_t pid;
    uint32_t tid;
    uint32_t data_register_value;
} t_write_mem_request;


void serialize_write_mem_request(t_write_mem_request* write_mem_request, t_buffer* buffer);

t_write_mem_request* deserialize_write_mem_request(void* stream);

t_write_mem_request* create_write_mem_request(uint32_t tid, uint32_t pid, uint32_t physical_address, uint32_t data_register_value) ;

void destroy_write_mem_request(t_write_mem_request* write_mem_request);

#endif

