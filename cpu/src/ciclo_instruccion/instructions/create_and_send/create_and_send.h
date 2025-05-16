#ifndef CREATE_AND_SEND_H
#define CREATE_AND_SEND_H

// Utils
#include "utils/communication.h"
#include "utils/estructuras.h"
#include "utils/server.h"

// Commons 
#include <commons/log.h>
#include <commons/collections/list.h>

#include <utils/serializators/syscalls_serialization/dump_memory_serialization/dump_memory_serialization.h>
#include <utils/serializators/syscalls_serialization/io_serialization/io_serialization.h>
#include <utils/serializators/syscalls_serialization/process_create_serialization/process_create_serialization.h>
#include <utils/serializators/syscalls_serialization/thread_create_serializtion/thread_create_serializtion.h>
#include <utils/serializators/syscalls_serialization/thread_operation_serializtion/thread_operation_serializtion.h>
#include <utils/serializators/syscalls_serialization/mutex_operation_serialization/mutex_operation_serialization.h>
#include <utils/serializators/syscalls_serialization/process_exit_serialization/process_exit_serialization.h>

void crear_enviar_process_create(char* nombre_archivo, uint32_t tamanio_proceso, uint32_t prioridad, int cliente_fd);
void crear_enviar_process_exit(uint32_t pid, int cliente_fd);
void crear_enviar_thread_create(char* nombre_archivo, uint32_t prioridad, int cliente_fd);
void crear_enviar_thread_operation(op_code operation, uint32_t tid, int cliente_fd);
void crear_enviar_mutex_operation(op_code operation, char* nombre_semaforo, uint32_t tid, int cliente_fd);
void crear_enviar_dump_memory(uint32_t pid, uint32_t tid, int cliente_fd);
void crear_enviar_syscall_io(uint32_t milisegundos, int cliente_fd);

#endif