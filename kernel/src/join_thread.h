#ifndef JOIN_THREAD_H
#define JOIN_THREAD_H

#include <tcb_lists.h>
#include <pthread.h>

// Estructura para representar un join
typedef struct {
    t_tcb *blocked_tcb;    // TCB bloqueado por el join
    t_tcb *blocking_tcb;   // TCB que debe terminar para desbloquear
} t_tcb_join;

// Lista global de joins y su mutex
extern t_list *list_tcb_joins;             
extern pthread_mutex_t mutex_list_tcb_joins;

// Funciones de manejo de joins
/**
 * @brief Inicializa la lista global de joins.
 */
void init_tcb_joins();

/**
 * @brief Agrega un join entre dos TCBs.
 * 
 * @param blocked_tcb TCB que será bloqueado.
 * @param blocking_tcb TCB que debe terminar para desbloquear.
 */
void add_tcb_join(t_tcb *blocked_tcb, t_tcb *blocking_tcb);

/**
 * @brief Desbloquea todos los TCBs que estaban bloqueados por un TCB terminado.
 * 
 * @param finished_tcb TCB que terminó su ejecución.
 */
void unlock_tcb_joins(t_tcb *finished_tcb);

/**
 * @brief Libera todos los recursos asociados a la lista global de joins.
 */
void destroy_tcb_joins();

#endif /* JOIN_THREAD_H */
