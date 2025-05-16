#ifndef CICLO_INSTRUCCION_H
#define CICLO_INSTRUCCION_H



// Utils
#include "utils/communication.h"
#include "utils/estructuras.h"
#include "utils/server.h"
#include "utils/message_serialization.h"
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"

// Commons 
#include <commons/log.h>
#include <commons/collections/list.h>

//
#include <mmu/mmu.h>
#include <ciclo_instruccion/instructions/instructions.h>
#include <execution_context/execution_context.h>

// Defines
#include <defines.h>

// Funciones principales
char* fetch(t_execution_context* execution_context, int memory_fd, int kernel_fd);
int decode_and_execute (char* instruction_char, t_execution_context* execution_context, int memory_fd, int kernel_fd);
void execute_instruction_cycle(t_execution_context *execution_context, int memory_fd, int kernel_fd);

// Funciones auxiliares
void send_dispatch_return(u_int32_t tid, u_int32_t pid, int kernel_fd, op_code code);
char* receive_instruction(int socket_fd);
op_code string_to_op_code(char* name);
void request_instruction_to_memory(t_execution_context* execution_context, int memory_fd) ;
t_instruction* parse_instruction(char* input);
void print_instruction(t_instruction* instruction);
op_code string_to_op_code(char* name);
uint32_t casting_char_to_uint32_t(char *param);

#endif