#ifndef CPU_INTERRUPT_H
#define CPU_INTERRUPT

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
#include "execution_context/execution_context.h"
#include "ciclo_instruccion/ciclo_instruccion.h"

// Funciones principales
void run_server_interrupt(void *arg);
void handle_client_interrupt(void *arg);


#endif