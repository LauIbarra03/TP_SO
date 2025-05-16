#ifndef UTILS_INSTRUCTION_REQUEST_SERIALIZATION_H_
#define UTILS_INSTRUCTION_REQUEST_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>

typedef struct {
    u_int32_t tid;
    u_int32_t pid;
    u_int32_t pc;
} t_instruction_request;


void serialize_instruction_request(t_instruction_request* request, t_buffer* buffer) ;

t_instruction_request* deserialize_instruction_request(void* stream);

t_instruction_request* create_instruction_request(u_int32_t tid, u_int32_t pid, u_int32_t pc);

void destroy_instruction_request(t_instruction_request* request);


#endif


