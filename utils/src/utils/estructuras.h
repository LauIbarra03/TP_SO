#ifndef UTILS_ESTRUCTURAS_H_
#define UTILS_ESTRUCTURAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <stdint.h> // Para tipos uint32_t
#include <semaphore.h>
#include <commons/collections/list.h>

// Definición de estados para TCBs y PCBs
typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} t_state;

// Definición de estructura para los registros de un TCB
typedef struct {
    uint32_t PC;
    uint32_t AX;
    uint32_t BX;
    uint32_t CX;
    uint32_t DX;
    uint32_t EX;
    uint32_t FX;
    uint32_t GX;
    uint32_t HX;
    uint32_t LIMITE;
    uint32_t BASE;
} t_register;

// Estructura para el mutex simulado en el PCB
typedef struct {
    char *name;             // Nombre del mutex
    uint32_t name_length;   // Longitud del nombre del mutex (para serialización/deserialización)
    uint32_t assigned_tid;  // TID del hilo que está reservando el mutex
    bool assigned;          // Indica si el mutex está actualmente asignado
} t_mutex_pcb;

// Estructura para un TCB (Thread Control Block)
typedef struct {
    uint32_t TID;               // Identificador del hilo
    uint32_t parent_PID;        // Identificador del pcb padre
    uint32_t priority;          // Prioridad del hilo
    t_state state;              // Estado del hilo
    char *file_path;            // Ruta de archivo en formato string
    uint32_t file_path_length;  // Longitud de la ruta de archivo
} t_tcb;

// Estructura para un PCB (Process Control Block)
typedef struct {
    uint32_t PID;               // Id entero autoincremental
    t_state state;              // Estado del proceso
    uint32_t *TID;              // Puntero a array de enteros
    uint32_t tid_count;         // Cantidad de TIDs
    t_mutex_pcb **mutex;         // Puntero a array de t_mutex_pcb
    uint32_t mutex_count;       // Cantidad de mutexes en el PCB
    t_tcb* tcbs;                // Puntero a array dinámico de t_tcb
    uint32_t num_tcbs;          // Cantidad de TCBs en el array
    uint32_t size;              // Tamaño en memoria del proceso
} t_pcb;

// Declaración de funciones generales
void handle_client(void *arg);
void run_server(void *arg);

#endif /* UTILS_ESTRUCTURAS_H_ */