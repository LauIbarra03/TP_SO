#ifndef KERNEL_DISPATCH_H
#define KERNEL_DISPATCH_H

#include <kernel.h>
#include <utils/communication.h>
#include <utils/estructuras.h>
#include <process.h>
#include <pcb_lists.h>
#include <tcb_lists.h>

// Serializacion de syscalls
#include <utils/serializators/syscalls_serialization/dump_memory_serialization/dump_memory_serialization.h>
#include <utils/serializators/syscalls_serialization/io_serialization/io_serialization.h>
#include <utils/serializators/syscalls_serialization/process_create_serialization/process_create_serialization.h>
#include <utils/serializators/syscalls_serialization/thread_create_serializtion/thread_create_serializtion.h>
#include <utils/serializators/syscalls_serialization/thread_operation_serializtion/thread_operation_serializtion.h>
#include <utils/serializators/syscalls_serialization/mutex_operation_serialization/mutex_operation_serialization.h>
#include <utils/serializators/syscalls_serialization/process_exit_serialization/process_exit_serialization.h>
#include <utils/serializators/seg_fault_serialization/seg_fault_serialization.h>
#include <syscalls_operations/thread_operations.h>
#include <mutexes/mutex_thread.h>
#include <syscalls_operations/memory_operations.h>
#include <io_device/io_device.h>

//


void enviar_pcb_y_esperar_respuesta(void* arg);
void dispatch_thread(t_tcb* tcb, int client_fd);

#endif