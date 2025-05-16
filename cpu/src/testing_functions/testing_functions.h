#ifndef CPU_TESTING_FUNCTIONS_H
#define CPU_TESTING_FUNCTIONS_H
/* Includes */ 

//Librerias de C
#include <pthread.h>

//Librerias de commons
#include <commons/log.h>
#include <commons/config.h>

//Carpeta local utils
#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>
#include <utils/estructuras.h>
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include "utils/serializators/dispatch_request/dispatch_request.h"
#include <utils/serializators/execution_context_serialization/execution_context_serialization.h>
#include <dispatch_cpu/dispatch_cpu.h>
#include <ciclo_instruccion/ciclo_instruccion.h>

#include <utils/serializators/syscalls_serialization/dump_memory_serialization/dump_memory_serialization.h>
#include <utils/serializators/syscalls_serialization/io_serialization/io_serialization.h>
#include <utils/serializators/syscalls_serialization/process_create_serialization/process_create_serialization.h>
#include <utils/serializators/syscalls_serialization/thread_create_serializtion/thread_create_serializtion.h>
#include <utils/serializators/syscalls_serialization/thread_operation_serializtion/thread_operation_serializtion.h>
#include <utils/serializators/syscalls_serialization/mutex_operation_serialization/mutex_operation_serialization.h>
#include <utils/serializators/syscalls_serialization/process_exit_serialization/process_exit_serialization.h>

// Funciones de testeo
void seg_fault_test(int kernel_fd, int memory_fd);
void update_execution_context_test(int memory_cpu_connection);
void request_execution_context_test(int memory_cpu_connection);
void fetch_test(int memory_fd, int kernel_fd) ;
void set_test(int memory_fd, int kernel_fd);
void sum_test(int memory_fd, int kernel_fd);
void sub_test(int memory_fd, int kernel_fd);
void log_test(int memory_fd, int kernel_fd);
void jnz_test(int memory_fd, int kernel_fd);
void read_mem_test(int memory_fd, int kernel_fd);
void write_mem_test(int memory_fd, int kernel_fd);
void dump_memory_test(int memory_fd, int kernel_fd);
void io_test(int memory_fd, int kernel_fd);
void process_create_test(int memory_fd, int kernel_fd);
void thread_create_test(int memory_fd, int kernel_fd);
void thread_join_test(int memory_fd, int kernel_fd);
void thread_cancel_test(int memory_fd, int kernel_fd);
void mutex_create_test(int memory_fd, int kernel_fd);
void mutex_lock_test(int memory_fd, int kernel_fd);
void mutex_unlock_test(int memory_fd, int kernel_fd);
void thread_exit_test(int memory_fd, int kernel_fd);
void process_exit_test(int memory_fd, int kernel_fd);

void test_dump(u_int32_t tid, u_int32_t pid , int memory_fd);
void test_io(u_int32_t milisegundo, int memory_fd);
void test_process_create(char* nombre, uint32_t i1, uint32_t i2, int memory_fd);
void test_thread_create(char* nombre, uint32_t prio, int memory_fd);
void test_thead_operation(uint32_t tid, int memory_fd);
void test_mutex_operation(char *nombre, uint32_t tid, int memory_fd);
void test_process_exit(uint32_t pid, int memory_fd);

#endif