#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

// Utils
#include "utils/communication.h"
#include "utils/estructuras.h"
#include "utils/server.h"
#include "utils/message_serialization.h"
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include "utils/serializators/execution_context_serialization/execution_context_serialization.h"
#include "utils/serializators/read_mem_request_serialization/read_mem_request_serialization.h"
#include "utils/serializators/write_mem_request_serialization/write_mem_request_serialization.h"
#include "create_and_send/create_and_send.h"

// Externas
#include <pthread.h>

// Commons 
#include <commons/log.h>
#include <commons/collections/list.h>

// MMU
#include <mmu/mmu.h>

// Defines
#include <defines.h>

// Structs
typedef enum {
    PC,
    AX,
    BX,
    CX,
    DX,
    EX,
    FX,
    GX,
    HX,
    INVALID_REGISTER 
} t_register_name;

// Variables
extern int interrupcion;
extern pthread_mutex_t mutex_interrupcion;

// Funciones de instrucciones
void set_instruction(char *param1, char *param2, t_execution_context* execution_context);
void sum_instruction(char *param1, char *param2, t_execution_context* execution_context);
void sub_instruction(char *param1, char *param2, t_execution_context* execution_context);
void log_instruction(char *param1, t_execution_context* execution_context);
void jnz_instruction(char *param1, char* param2, t_execution_context* execution_context);
int write_mem_instruction(char *param1, char *param2, t_execution_context* execution_context, int memory_fd, int kernel_fd);
int read_mem_instruction(char *param1, char *param2, t_execution_context* execution_context, int memory_fd, int kernel_fd);

// Funciones auxiliares
t_register_name return_register(char* reg);
void set_value(t_register_name register_name, u_int32_t value, t_execution_context* execution_context);
uint32_t get_register_value(t_register_name register_name, t_execution_context* execution_context);
void request_read_mem_to_memory(t_execution_context* execution_context, int address, int memory_fd);
void request_write_mem_to_memory(t_execution_context* execution_context, int address, uint32_t data,int memory_fd);
uint32_t receive_read_mem_data(int memory_fd);


#endif