#ifndef UTILS_SERVER_H_
#define UTILS_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>

extern t_log* logger;

/**
* @fn     start_server
* @brief  Inicia el servidor y devuelve el socket del servidor
* @param  const char* port: El número de puerto al cual se va a asociar el servidor
* @return int: El descriptor de socket del servidor, o -1 en caso de fallo
*/
int start_server(const char*);

/**
* @fn     wait_for_client
* @brief  Espera a que un cliente se conecte y acepta la conexión
* @param  int server_socket: El descriptor de socket del servidor
* @return int: El descriptor de socket del cliente, o -1 en caso de fallo
*/
int wait_for_client(int);

/**
* @fn     receive_package
* @brief  Recibe un paquete del cliente deserializandolo y devolviendo una lista de items.
* @param  int client_socket: El descriptor de socket del cliente
* @return t_list*: Una lista de valores recibidos (cadenas de texto)
*/
t_list* receive_package(int);

/**
* @fn     receive_message
* @brief  Recibe un mensaje del cliente
* @param  int client_socket: El descriptor de socket del cliente
* @return void
*/
void receive_message(int);

/**
* @fn     receive_operation_code
* @brief  Recibe un código de operación del cliente
* @param  int client_socket: El descriptor de socket del cliente
* @return int: El código de operación, o -1 si el cliente se desconecta
*/
int receive_operation_code(int);

/**
* @fn     receive_buffer
* @brief  Recibe un buffer del cliente
* @param  int* size: Puntero para almacenar el tamaño del buffer
* @param  int client_socket: El descriptor de socket del cliente
* @return void*: El buffer recibido
*/
void* receive_buffer(int*, int);

t_list* receive_package(int client_socket);

void* receive_stream(int client_socket, int* total_size);

#endif /* UTILS_SERVER_H_ */
