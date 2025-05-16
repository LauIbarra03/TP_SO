#ifndef CPU_DISPATCH_CPU_H
#define CPU_DISPATCH_CPU_H

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
#include <ciclo_instruccion/ciclo_instruccion.h>
#include "utils/serializators/instruction_request_seriailization/instruction_request_seriailization.h"
#include "utils/serializators/instruction_serialization/instruction_serialization.h"
#include "utils/serializators/dispatch_request/dispatch_request.h"
#include "utils/serializators/execution_context_serialization/execution_context_serialization.h"
#include <testing_functions/testing_functions.h>

// Defines
#include <defines.h>

// Variables

// Funciones principales
void handle_client_cpu_dispatch(void *arg);
void run_server_dispatch_cpu(void *arg);





#endif