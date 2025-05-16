#ifndef UTILS_PCB_SERIALIZATION_H_
#define UTILS_PCB_SERIALIZATION_H_

#include <utils/estructuras.h>
#include <utils/communication.h>

extern uint32_t global_pid;

/**
 * @brief Crea un nuevo PCB (Process Control Block) asignando sus arrays y valores iniciales.
 * 
 * @param size Tamaño en memoria del proceso.
 * 
 * @return Un puntero al PCB recién creado con los valores y estructuras inicializados.
 *         El array de TCBs (hilos) y el array de TID se inicializan vacíos y se pueden
 *         llenar utilizando la función add_tcb_to_pcb.
 */
t_pcb* create_pcb(uint32_t size);


/**
 * @brief Serializa un PCB en un buffer para poder enviarlo a través de comunicación.
 * 
 * @param pcb Un puntero al PCB a ser serializado.
 * @param buffer Un puntero al buffer donde se almacenará el PCB serializado.
 */
void serialize_pcb(t_pcb* pcb, t_buffer* buffer);

/**
 * @brief Deserializa un stream de datos en un PCB.
 * 
 * @param stream El stream de datos que contiene la información serializada del PCB.
 * 
 * @return Un puntero al PCB deserializado.
 */
t_pcb* deserialize_pcb(void* stream);

/**
 * @brief Libera la memoria asignada a un PCB.
 * 
 * @param pcb Un puntero al PCB que debe ser liberado.
 */
void delete_pcb(t_pcb* pcb);

#endif
