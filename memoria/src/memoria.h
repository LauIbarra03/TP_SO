#ifndef MEMORIA_MEMORIA_H
#define MEMORIA_MEMORIA_H

/* Includes */

// Librerías de C
#include <pthread.h>

// Librerías de commons
#include <commons/log.h>
#include <commons/config.h>

// Utils locales
#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>
#include <utils/estructuras.h>
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include <utils/pcb_serialization.h>
#include <utils/tcb_serialization.h>
#include <utils/serializators/read_mem_request_serialization/read_mem_request_serialization.h>
#include <utils/serializators/write_mem_request_serialization/write_mem_request_serialization.h>
#include <utils/serializators/syscalls_serialization/dump_memory_serialization/dump_memory_serialization.h>
#include <utils/serializators/pcb_request/pcb_request_serialization.h>

// Módulo de memoria
#include "mem_structs.h"
#include "mem_buffer.h"
#include <partitions/partitions.h>
#include <partitions/mem_dynamic_partition.h>

// Serializadores de kernel
#include <utils/serializators/execution_context_serialization/execution_context_serialization.h>
#include <utils/serializators/dispatch_request/dispatch_request.h>
#include <utils/serializators/syscalls_serialization/dump_memory_serialization/dump_memory_serialization.h>


/* Declaración de Variables */ 

extern int server_fd;
extern int* socket_server;

extern t_log *logger;
extern t_config *config;

extern pthread_t server_thread;

extern char* puerto_server;

// Lista de particiones
extern t_list *partition_list;

// Tamaño de la memoria y buffer de usuario
extern uint32_t TAM_MEMORIA;
extern void* memoria_usuario;

/* Declaración de Funciones */

// Función para manejar respuestas del filesystem
char *receive_fs_response(int socket_cliente);
void send_response_to_cpu(char *mensaje, int socket_cliente);
op_code send_dump_memory(t_dump_context* context);

// Funciones auxiliares
void destroy_string_array(char** array);

// Funciones de testing
void test_send_dump_request(int conexion_filesystem);
void test_allocateProcessInPartition(t_list *partition_list);

#endif
