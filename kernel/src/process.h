#ifndef KERNEL_PROCESS_H_
#define KERNEL_PROCESS_H_

#include <stdlib.h>
#include <stdio.h>
#include <utils/estructuras.h>
#include <utils/pcb_serialization.h>
#include <utils/tcb_serialization.h>
#include <utils/serializators/pcb_request/pcb_request_serialization.h>

/**
 * @brief Inicializa un proceso de prueba.
 * 
 * @return t_pcb* Puntero al PCB inicializado, o NULL si no se pudo crear.
 */
t_pcb *initialize_process();

/**
 * @brief Registra información detallada sobre los hilos asociados a un PCB.
 * 
 * @param pcb Puntero al PCB cuyo contenido se desea registrar en el log.
 */
void log_pcb_threads_info(t_pcb* pcb);

/**
 * @brief Registra los TIDs asociados a un PCB en el log.
 * 
 * @param pcb Puntero al PCB cuyos TIDs se desean registrar.
 */
void log_pcb_tids(t_pcb* pcb);

/**
 * @brief Crea un proceso y un hilo inicial (TID 0) tanto en el kernel como en memoria.
 * 
 * @param size Tamaño del proceso.
 * @param path Ruta del archivo asociado al proceso.
 * @param priority Prioridad del proceso.
 */
op_code create_process(uint32_t size, char* path, int priority);

/**
 * @brief Crea un hilo asociado a un PCB tanto en el kernel como en memoria.
 * 
 * @param pcb Puntero al PCB al que se asociará el nuevo hilo.
 * @param path Ruta del archivo asociado al hilo.
 * @param priority Prioridad del hilo.
 * @return t_tcb* Puntero al TCB creado, o NULL si ocurrió un error.
 */
t_tcb *create_thread(t_pcb* pcb, char* path, int priority);

/**
 * @brief Crea una estructura de mutex con un nombre específico.
 * 
 * @param name Nombre del mutex.
 * @return t_mutex_pcb* Puntero al mutex creado, o NULL si ocurrió un error.
 */
t_mutex_pcb* create_mutex(const char* name);

/**
 * @brief Agrega un mutex a la lista de mutexes de un PCB.
 * 
 * @param pcb Puntero al PCB al que se añadirá el mutex.
 * @param mutex Puntero al mutex que se desea agregar.
 */
//void add_mutex_to_pcb(t_pcb* pcb, t_mutex_pcb* mutex);

/**
 * @brief Registra información detallada sobre los mutexes asociados a un PCB.
 * 
 * @param pcb Puntero al PCB cuyos mutexes se desean registrar.
 */
void log_mutex_pcb(t_pcb *pcb);

/**
 * @brief Envía una solicitud para eliminar un proceso en memoria.
 * 
 * @param pcb Puntero al PCB que se desea eliminar en memoria.
 * @return op_code Resultado de la operación (OK o ERROR).
 */
op_code delete_process_memory(t_pcb *pcb);

/**
 * @brief Envía una solicitud para eliminar un hilo en memoria.
 * 
 * @param tcb Puntero al TCB que se desea eliminar en memoria.
 * @return op_code Resultado de la operación (OK o ERROR).
 */
op_code delete_thread_memory(t_tcb *tcb);

#endif /* KERNEL_PROCESS_H_ */
