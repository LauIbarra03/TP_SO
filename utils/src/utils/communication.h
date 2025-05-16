#ifndef UTILS_COMMUNICATION_H_
#define UTILS_COMMUNICATION_H_

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <utils/server.h>

typedef enum {
    //Generic Operation Codes
    OK,
    ERROR,
    MESSAGE,
    DISCONNECT,
    
    //Kernel -> Cpu
    KERNEL_INTERRUPTION,
    QUANTUM_INTERRUPTION,
    DISPATCH,

    //Cpu -> Kernel
    IO,
    THREAD_JOIN,

    MUTEX_CREATE,
    MUTEX_LOCK,
    MUTEX_UNLOCK,

    //Cpu -> Kernel -> Memory
    THREAD_CANCEL,
    PROCESS_CREATE,
    THREAD_CREATE,

    //Cpu -> Kernel -> Memory -> FS
    DUMP_MEMORY,

    //Cpu -> Memoria
    FETCH,

    //Cpu -> Memoria
    //Cpu -> Kernel
    THREAD_EXIT,
    PROCESS_EXIT,

    //Memoria -> Cpu
    EXECUTION_CONTEXT,
    
    //Hilos

    // Instrucciones
    SET,
    READ_MEM,
    WRITE_MEM,
    SUM,
    SUB,
    JNZ,
    LOG,

    INSTRUCTION_FROM_MEMORY,
    INSTRUCTION_REQUEST,
    EXECUTION_CONTEXT_UPDATE,
    READ_MEM_REQUEST,
    WRITE_MEM_REQUEST,
    // 
    SEGMENTATION_FAULT
} op_code;

typedef struct {
    op_code codigo_respuesta;
} t_respuesta_syscall;

typedef struct {
    uint32_t size;
    uint32_t offset;
    void *stream;
} t_buffer;
typedef struct {
    op_code operation_code;
    t_buffer *buffer;
} t_paquete;


/*
Memory
En este apartado solamente se tendrá la instrucción DUMP_MEMORY. Esta syscall le solicita a la memoria, 
junto al PID y TID que lo solicitó, que haga un Dump del proceso.
Esta syscall bloqueará al hilo que la invocó hasta que el módulo memoria confirme la finalización de la operación, 
en caso de error, el proceso se enviará a EXIT. Caso contrario, el hilo se desbloquea normalmente pasando a READY.
Entrada Salida
Para la implementación de este trabajo práctico, el módulo Kernel simulará la existencia de un único dispositivo de 
Entrada Salida, el cual atenderá las peticiones bajo el algoritmo FIFO. Para “utilizar” esta interfaz, se dispone de 
la syscall IO. Esta syscall recibe como parámetro la cantidad de milisegundos que el hilo va a permanecer haciendo la 
operación de entrada/salida.

*/




/**
* @brief Crea una conexión con un servidor utilizando la IP y el puerto proporcionados.
* @param ip Dirección IP del servidor.
* @param port Puerto del servidor.
* @return Descriptor del socket de la conexión, o -1 si la conexión falla.
*/
int create_connection(char *ip, char *port);

/**
* @brief Crea un paquete con el código de operación especificado.
* @param op Código de operación a incluir en el paquete.
* @return Puntero al paquete creado.
*/
t_paquete *create_package(op_code op);

/**
* @brief Agrega datos al buffer de un paquete.
* @param package Puntero al paquete.
* @param value Puntero a los datos a agregar.
* @param size Tamaño de los datos en bytes.
* @return No tiene valor de retorno.
*/
void add_to_package(t_paquete *package, void *value, int size);
/**
* @brief Envía un paquete a través de un socket especificado.
* @param package Puntero al paquete a enviar.
* @param client_socket Descriptor del socket para enviar el paquete.
* @return No tiene valor de retorno.
*/
void send_package(t_paquete *package, int client_socket);

/**
* @brief Cierra la conexión representada por un descriptor de socket.
* @param client_socket Descriptor del socket de la conexión a cerrar.
* @return No tiene valor de retorno.
*/
void free_connection(int client_socket);

/**
* @brief Libera la memoria asignada para un paquete.
* @param package Puntero al paquete a eliminar.
* @return No tiene valor de retorno.
*/
void delete_package(t_paquete *package);

/**
* @brief Crea una conexión utilizando los valores de IP y puerto de un archivo de configuración.
* @param config Puntero a la estructura de configuración.
* @param ip_config Clave del valor de IP en el archivo de configuración.
* @param port_config Clave del valor del puerto en el archivo de configuración.
* @return Descriptor del socket de la conexión, o -1 si la conexión falla.
*/
int connection_by_config(t_config *config, char *ip_config, char *port_config);

/**
* @brief Envía un mensaje como un paquete a través de un socket especificado.
* @param message Puntero a la cadena de mensaje.
* @param client_socket Descriptor del socket para enviar el mensaje.
* @return No tiene valor de retorno.
*/
void send_message(char *message, int client_socket);

/**
* @brief Serializa un paquete en un flujo de bytes para su transmisión.
* @param package Puntero al paquete a serializar.
* @param bytes Tamaño total de los datos serializados en bytes.
* @return Puntero a los datos serializados.
*/
void* serialize_package(t_paquete *paquete, int bytes); 
/**
* @brief Inicializa un buffer dentro de un paquete.
* @param package Puntero al paquete para inicializar el buffer.
* @return No tiene valor de retorno.
*/
void create_buffer(t_paquete *package);

/**
* @brief Envía un código de respuesta a través de un socket especificado.
* @param client_socket Descriptor del socket para enviar la respuesta.
* @param code Código de operación a enviar como respuesta.
* @return No tiene valor de retorno.
*/
void send_response(int client_socket, op_code code);

op_code wait_for_response(int client_socket);
t_paquete* deserealizar_paquete(void* stream);
char *receive_memory_response(int socket_cliente);
void destroy_buffer(t_buffer* buffer);

#endif