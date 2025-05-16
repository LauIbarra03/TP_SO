#ifndef CPU_EXECUTION_CONTEXT_H
#define CPU_EXECUTION_CONTEXT_H

// Utils
#include "utils/communication.h"
#include "utils/estructuras.h"
#include "utils/server.h"
#include "utils/message_serialization.h"
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include "utils/serializators/execution_context_serialization/execution_context_serialization.h"
#include "utils/serializators/dispatch_request/dispatch_request.h"

// Commons 
#include <commons/log.h>
#include <commons/collections/list.h>

// Funciones
void request_execution_context(u_int32_t tid, u_int32_t pid , int memory_fd);
t_execution_context* receive_execution_context(int socket_cliente);
void update_execution_context(t_execution_context* execution_context, int memory_fd);



#endif