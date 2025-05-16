#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

//Librerias de C
#include <pthread.h>

//Librerias de commons
#include <commons/log.h>
#include <commons/config.h>

//Carpeta local utils
#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>

#include <fileSystemOperations/operationsFS.h>

#include <utils/estructuras.h>
#include <utils/estructuras.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <utils/serializators/execution_context_serialization/execution_context_serialization.h>

#include <fileSystemOperations/operationsFS.h>

typedef struct{
    t_dump_context* dump_context;
    int client_fd;
} t_estructura_para_dump;


/* Declaracion de Variables */

extern int server_fd;
extern int* socket_server;

extern t_log *logger;
extern t_config *config;

extern pthread_t server_thread;

extern char* puerto_server;

// Funciones 
void send_response_to_memory(op_code codigo_respuesta, int socket_cliente);

void ejecutar_dump_memory(void* arg);

void free_estructura_dump(t_estructura_para_dump* estructura);



#endif