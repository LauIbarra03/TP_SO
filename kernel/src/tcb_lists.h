#ifndef TCB_LISTS_H_
#define TCB_LISTS_H_

#include <commons/collections/list.h>
#include <utils/estructuras.h>
#include <pthread.h>

#include "scheduler.h"

// Declaración de listas para TCBs
extern t_list *list_NEW_TCB;
extern t_list *list_READY_TCB;
extern t_list *list_BLOCKED_TCB;
extern t_list *list_EXIT_TCB;

extern pthread_mutex_t mutex_list_NEW_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB;

extern t_tcb* current_tcb_executing;

// Funciones para inicializar y destruir las listas de TCBs

/**
 * @brief Inicializa las listas de TCBs (NEW, READY, BLOCKED, EXIT).
 */
void init_tcb_lists();

/**
 * @brief Destruye todas las listas de TCBs y libera los recursos asociados.
 */
void destroy_tcb_lists();

// Funciones para manejar TCBs en listas

/**
 * @brief Añade un TCB a la lista especificada.
 * 
 * @param list Puntero a la lista a la que se añadirá el TCB.
 * @param tcb Puntero al TCB que se añadirá a la lista.
 */
void add_tcb_to_list(t_list *list, t_tcb *tcb);

/**
 * @brief Elimina y devuelve un TCB de la lista, identificado por el `PID` del proceso padre y el `TID` del TCB.
 * 
 * @param list Puntero a la lista de donde se eliminará el TCB.
 * @param pid Identificador del proceso padre.
 * @param tid Identificador del TCB.
 * @return Puntero al TCB eliminado o `NULL` si no se encuentra.
 */
t_tcb *remove_tcb_from_list(t_list *list, uint32_t pid, uint32_t tid);

/**
 * @brief Busca un TCB en la lista, identificado por el `PID` del proceso padre y el `TID` del TCB.
 * 
 * @param list Puntero a la lista donde se buscará el TCB.
 * @param pid Identificador del proceso padre.
 * @param tid Identificador del TCB.
 * @return Puntero al TCB encontrado o `NULL` si no se encuentra.
 */
t_tcb *find_tcb_in_list(t_list *list, uint32_t pid, uint32_t tid);

/**
 * @brief Devuelve el número de TCBs en la lista especificada.
 * 
 * @param list Puntero a la lista de TCBs.
 * @return Número de TCBs en la lista.
 */
int count_tcbs_in_list(t_list *list);

/**
 * @brief Mueve un TCB de una lista a otra y actualiza su estado.
 * 
 * @param src_list Lista de origen desde donde se moverá el TCB.
 * @param dest_list Lista de destino a donde se moverá el TCB.
 * @param pid Identificador del proceso padre.
 * @param tid Identificador del TCB.
 * @param new_state Nuevo estado que se asignará al TCB.
 */
void move_tcb_between_lists(t_list *src_list, t_list *dest_list, uint32_t pid, uint32_t tid, t_state new_state);

/**
 * @brief Registra el contenido de una lista de TCBs en el logger.
 * 
 * @param list Puntero a la lista cuyo contenido se registrará.
 * @param list_name Nombre de la lista que se usará en el log.
 */
void log_list_contents_tcb(t_list *list, const char *list_name);

/**
 * @brief Elimina el primer TCB de la lista y lo devuelve.
 * 
 * @param list Puntero a la lista de donde se eliminará el primer TCB.
 * @return Puntero al primer TCB eliminado o `NULL` si la lista está vacía.
 */
t_tcb *list_pop_first_tcb(t_list *list);

/**
 * @brief Elimina el último TCB de la lista y lo devuelve.
 * 
 * @param list Puntero a la lista de donde se eliminará el último TCB.
 * @return Puntero al último TCB eliminado o `NULL` si la lista está vacía.
 */
t_tcb *list_pop_last_tcb(t_list *list);


int existing_tcb(uint32_t ppid, uint32_t tid);
void block_tcb(t_tcb* tcb);
t_tcb* find_tcb_from_tid(uint32_t tid, uint32_t ppid);


#endif /* TCB_LISTS_H_ */
