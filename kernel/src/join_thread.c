#include "join_thread.h"
#include <pthread.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <tcb_lists.h>

extern t_log *logger;

extern sem_t sem_thread_count;

t_list *list_tcb_joins = NULL;                                      // Lista global de joins
pthread_mutex_t mutex_list_tcb_joins = PTHREAD_MUTEX_INITIALIZER;   // Mutex para la lista

void init_tcb_joins() {
    pthread_mutex_lock(&mutex_list_tcb_joins);
    list_tcb_joins = list_create();
    pthread_mutex_unlock(&mutex_list_tcb_joins);
    log_info(logger, "Lista de joins inicializada.");
}

void add_tcb_join(t_tcb *blocked_tcb, t_tcb *blocking_tcb) {
    t_tcb_join *new_join = malloc(sizeof(t_tcb_join));
    if (new_join == NULL) {
        log_error(logger, "No se pudo asignar memoria para un nuevo join.");
        return;
    }

    new_join->blocked_tcb = blocked_tcb;
    new_join->blocking_tcb = blocking_tcb;

    pthread_mutex_lock(&mutex_list_tcb_joins);
    list_add(list_tcb_joins, new_join);  // Añadir a la lista de joins
    pthread_mutex_unlock(&mutex_list_tcb_joins);

    // Añadir el TCB bloqueado a la lista de bloqueados
    pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
    log_info(logger, "## (PID:TID) : (%i:%i) Bloqueado por: PTHREAD_JOIN", blocked_tcb->parent_PID, blocked_tcb->TID);
    add_tcb_to_list(list_BLOCKED_TCB, blocked_tcb);
    pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

    log_info(logger, "TCB TID=%d bloqueado esperando a TID=%d.", blocked_tcb->TID, blocking_tcb->TID);
}


void unlock_tcb_joins(t_tcb *finished_tcb) {
    pthread_mutex_lock(&mutex_list_tcb_joins);

    for (int i = 0; i < list_size(list_tcb_joins); i++) {
        t_tcb_join *join = list_get(list_tcb_joins, i);

        if (join->blocking_tcb->TID == finished_tcb->TID &&
            join->blocking_tcb->parent_PID == finished_tcb->parent_PID) {
            
            // Mover el TCB bloqueado de BLOCKED a READY
            pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
            pthread_mutex_lock(&mutex_list_READY_TCB);
            move_tcb_between_lists(list_BLOCKED_TCB, list_READY_TCB, join->blocked_tcb->parent_PID, join->blocked_tcb->TID, READY);
            pthread_mutex_unlock(&mutex_list_READY_TCB);
            pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

            log_info(logger, "TCB TID=%d desbloqueado porque TID=%d terminó.", 
                     join->blocked_tcb->TID, finished_tcb->TID);

            sem_post(&sem_thread_count);

            // Eliminar el join de la lista
            list_remove_and_destroy_element(list_tcb_joins, i, free);
            i--;  // Ajustar índice tras la eliminación
        }
    }

    pthread_mutex_unlock(&mutex_list_tcb_joins);
}

void destroy_tcb_joins() {
    pthread_mutex_lock(&mutex_list_tcb_joins);
    list_destroy_and_destroy_elements(list_tcb_joins, free);
    pthread_mutex_unlock(&mutex_list_tcb_joins);
    log_info(logger, "Lista de joins destruida.");
}
