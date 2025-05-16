#ifndef KERNEL_KERNEL_H
#define KERNEL_KERNEL_H

/* Includes */

//Librerias de C
#include <pthread.h>

//Librerias de commons
#include <commons/log.h>
#include <commons/config.h>

//Carpeta local utils
#include <utils/estructuras.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>
#include <utils/server.h>
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include "utils/serializators/dispatch_request/dispatch_request.h"
#include <kernel_dispatch/kernel_dispatch.h>
#include <kernel_interrupt/kernel_interrupt.h>
#include <tests_syscalls/thread_tests/thread_test.h>

typedef struct {
    t_config* config;
    t_pcb* pcb;
} t_init_modulo;

/* Declaracion de Variables */

extern int server_fd;
extern int* socket_server;

extern int quantum_interruption;
extern int kernel_interruption;

extern t_log *logger;
extern t_config *config;

extern pthread_t server_escucha_kernel_thread;
extern pthread_t connection_dispatch_thread;
extern pthread_t connection_interrupt_thread;

extern sem_t mutex_kernel_interruption;
extern sem_t mutex_quantum_interruption;


//extern char* puerto_server;

/* Declaracion de Funciones */

void run_server(void* arg);
void run_server_dispatch(void* arg);
void handle_client(void* arg);
void handle_client_cpu(void* arg);
void connect_cpu_dispatch(void* arg);

#endif