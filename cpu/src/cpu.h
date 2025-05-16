#ifndef CPU_CPU_H
#define CPU_CPU_H

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

// Carpeta del propio modulo 
#include <ciclo_instruccion/ciclo_instruccion.h>
#include <dispatch_cpu/dispatch_cpu.h>
#include <interrupt_cpu/interrupt_cpu.h>
#include <testing_functions/testing_functions.h>

extern pthread_t server_thread_interrupt;
extern pthread_t server_thread_dispatch;

t_log *logger;
t_config *config;

extern pthread_t server_thread_interrupt;
extern pthread_t server_thread_dispatch;

// Funciones auxiliares
int initializer();
void message_sending_example(int socket);





#endif

