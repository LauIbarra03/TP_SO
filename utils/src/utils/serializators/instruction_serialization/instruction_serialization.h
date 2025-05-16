#ifndef UTILS_INSTRUCTION_SERIALIZATION_H_
#define UTILS_INSTRUCTION_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>

typedef struct {
    op_code name;
    u_int32_t params_count;
    t_list *params;
} t_instruction;


void serialize_instruction(t_instruction* instruction, t_buffer* buffer) ;

t_instruction* deserialize_instruction(void* stream);

t_instruction* create_instruction(op_code name, t_list *params);

void destroy_instruction(t_instruction* instruction) ;


#endif

