#ifndef MEM_STRUCTS_H
#define MEM_STRUCTS_H

#include <stdint.h> // Para usar uint32_t
#include <commons/log.h> // Para usar t_log
#include <commons/collections/list.h> // Para usar t_list

#include <utils/communication.h>
#include <utils/estructuras.h>

#include <../../memoria/src/partitions/mem_dynamic_partition.h>
#include <utils/serializators/execution_context_serialization/execution_context_serialization.h>

extern t_log* logger;

extern uint32_t TAM_MEMORIA;
extern void* memoria_usuario;

// Definir la lista global de procesos en memoria
extern t_list* memory_process_list;

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
} t_mem_register;

/*
    Tener en cuenta que la base y el limite son derivados del size.
    Ambas cosas son para el espacio de memoria de usuario.
*/

typedef struct {
    uint32_t PID;           // Identificador único del proceso.
    uint32_t base;          // Dirección base de la partición de memoria asignada.
    uint32_t limite;        // Tamaño máximo de la partición (dirección base + tamaño).
    uint32_t size;          // Tamaño de la memoria de usuario asignada al proceso.
    t_mem_register regs;    // Registros asociados al proceso.
    t_list* threads;        // Lista de hilos asociados (t_mem_tcb*).
} t_mem_pcb;

typedef struct {
    uint32_t PID;     // PID del proceso, -1 si está libre
    uint32_t base;    // Dirección base de la partición en la memoria
    uint32_t limite;  // Dirección límite (base + tamaño) de la partición
    uint32_t size;    // Tamaño de la partición
    int isFree;       // 1 si está libre, 0 si está ocupada
} t_mem_partition;

typedef struct {
    uint32_t TID;            // Identificador único del hilo.
    t_mem_register regs;     // Registros asociados al hilo.
    char* file_path;         // Ruta del archivo de pseudocódigo.
    uint32_t file_path_len;  // Tamaño de cadena de caracteres 
} t_mem_tcb;

// Declaración de la función para inicializar los registros
void init_registers(t_mem_register* regs);

/**
 * @brief Crea un nuevo proceso (PCB) y lo agrega a la lista global de procesos en memoria.
 * 
 * @param pid Identificador único del proceso.
 * @param base Dirección base de la partición de memoria asignada.
 * @param limite Tamaño máximo de la partición (dirección base + tamaño).
 * @param size Tamaño de la memoria de usuario asignada al proceso.
 * @return t_mem_pcb* Puntero al proceso creado.
 */
t_mem_pcb* crear_proceso_mem(uint32_t pid, uint32_t base, uint32_t limite, uint32_t size);

/**
 * @brief Elimina un proceso (PCB) de la lista global de procesos en memoria.
 * Si el sistema es dinámico, actualiza las particiones. 
 * Si es estático, libera la partición correspondiente.
 * 
 * @param pid Identificador único del proceso a eliminar.
 */
void eliminar_proceso_mem(uint32_t pid);

/**
 * @brief Agrega un nuevo hilo (TCB) a un proceso existente.
 * 
 * @param proceso Puntero al proceso (PCB) al que se va a agregar el hilo.
 * @param tid Identificador único del hilo.
 * @param file_path Ruta del archivo de pseudocódigo que ejecutará el hilo.
 * @return t_mem_tcb* Puntero al hilo creado.
 */
t_mem_tcb* agregar_hilo_mem(t_mem_pcb* proceso, uint32_t tid, const char* file_path);

/**
 * @brief Elimina un hilo (TCB) de un proceso existente.
 * 
 * @param pid Identificador único del proceso.
 * @param tid Identificador único del hilo a eliminar.
 */
void eliminar_hilo_mem(uint32_t pid, uint32_t tid);

/**
 * @brief Verifica si hay espacio para crear el proceso, y si es asi lo crea
 * 
 * @param pid PID del proceso
 * @param size Tamaño del proceso en memoria
 * @param tamanio_mem Tamaño total de la memoria
 * @return const char* Respuesta sobre si se pudo o no crear el proceso
 */
const char* verificar_espacio_y_crear_proceso(uint32_t pid, uint32_t size);

/**
 * @brief Agrega un hilo a un proceso existente
 * 
 * @param pid PID del proceso existente al cual se le agrega dicho hilo
 * @param tid TID del hilo a agregar
 * @param file_path ruta del archivo de pseudocodigo que acompaña al hilo
 * @return const char* Respuesta sobre si se pudo o no agregar el hilo
 */
const char* verificar_y_agregar_hilo(uint32_t pid, uint32_t tid, const char* file_path);

/**
 * @brief Envia una respuesta al modulo que peticiono algo
 * 
 * @param operation_code Codigo de la respuesta a enviarle a dicho modulo
 * @param fd File Descriptor del modulo receptor
 */
void enviar_respuesta_ante_peticion(op_code operation_code, uint32_t fd);

/**
 * @brief Busca los registros de un tcb y se los retorna a CPU
 * 
 * @param pid PID del proceso padre
 * @param tid TID del hilo que contiene los registros pedidos
 */
t_register* obtener_registros_tcb(uint32_t pid, uint32_t tid);

/**
 * @brief Busca la linea pedida y la retorna
 * 
 * @param pid PID del proceso 
 * @param tid TID del hilo que contiene la linea pedida
 * @param pc Program Counter o nro de la linea a buscar
 */
char* obtener_linea_pseudocodigo(u_int32_t pid, u_int32_t tid, u_int32_t pc);

void send_instruction(char *mensaje, int socket_cliente);

void send_execution_context(t_execution_context* execution_context, int socket_cliente);

void send_read_mem_data(uint32_t data_value, int socket_cliente);

/**
 * @brief Actualizar registros de un determinado proceso e hilo
 * 
 * @param pid PID del proceso
 * @param tid TID del hilo asociado al proceso
 * @param registros_actualizados Registros actualizados a asignar
 */
void actualizar_registros(u_int32_t pid, u_int32_t tid, t_register* registros_actualizados);

t_mem_pcb* buscar_proceso_por_pid(uint32_t pid);

void* obtener_memoria_proceso(uint32_t pid);

void log_memory_process_list();

void free_mem_pcb(t_mem_pcb* pcb);

#endif /* MEM_STRUCTS_H */
