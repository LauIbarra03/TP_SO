#ifndef UTILS_TCB_SERIALIZATION_H_
#define UTILS_TCB_SERIALIZATION_H_

#include <utils/estructuras.h>
#include <utils/communication.h>
#include <stdint.h> // Para usar uint32_t
#include <../../kernel/src/tcb_lists.h>

extern t_log* logger;

/**
 * @brief Serializa un TCB (Thread Control Block) en un buffer.
 * 
 * @param tcb Un puntero al TCB que será serializado.
 * @param buffer Un puntero al buffer donde se almacenará la información serializada.
 */
void serialize_tcb(t_tcb* tcb, t_buffer* buffer);

/**
 * @brief Deserializa un stream de datos en un TCB.
 * 
 * @param stream El stream de datos que contiene la información serializada del TCB.
 * @return Un puntero al TCB deserializado.
 */
t_tcb* deserialize_tcb(void* stream);

/**
 * @brief Crea un nuevo TCB (Thread Control Block).
 * 
 * @param TID El identificador del hilo (Thread ID) que será asignado al TCB.
 * @param priority La prioridad del hilo.
 * @param file_path La ruta del archivo asociada con el TCB.
 * @return Un puntero al TCB recién creado.
 */
t_tcb* create_tcb(uint32_t TID, uint32_t priority, const char *file_path);

void delete_tcb(t_tcb* tcb);

/**
 * @brief Elimina un TCB y libera su memoria.
 * 
 * @param tcb Un puntero al TCB que será destruido.
 */
void destroy_tcb(t_tcb* tcb);

#endif /* UTILS_TCB_SERIALIZATION_H_ */
