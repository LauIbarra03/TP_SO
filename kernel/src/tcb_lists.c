#include "tcb_lists.h"
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <utils/tcb_serialization.h>
#include <kernel.h>


// Inicializar las listas
t_list *list_NEW_TCB = NULL;
t_list *list_READY_TCB = NULL;
t_list *list_BLOCKED_TCB = NULL;
t_list *list_EXIT_TCB = NULL;

t_tcb* current_tcb_executing = NULL;


void init_tcb_lists() {
    list_NEW_TCB = list_create();
    list_READY_TCB = list_create();
    list_BLOCKED_TCB = list_create();
    list_EXIT_TCB = list_create();
}

void destroy_tcb_lists() {
    list_destroy_and_destroy_elements(list_NEW_TCB, (void *)destroy_tcb);
    list_destroy_and_destroy_elements(list_READY_TCB, (void *)destroy_tcb);
    list_destroy_and_destroy_elements(list_BLOCKED_TCB, (void *)destroy_tcb);
    list_destroy_and_destroy_elements(list_EXIT_TCB, (void *)destroy_tcb);
}

void add_tcb_to_list(t_list *list, t_tcb *tcb) {
    list_add(list, tcb);
}

// Función que elimina y devuelve el primer TCB de la lista
t_tcb *list_pop_first_tcb(t_list *list) {
    if (list_size(list) == 0) {
        return NULL; // Si la lista está vacía
    }
    return list_remove(list, 0); // Remover y retornar el primer elemento
}

// Función que elimina y devuelve el último TCB de la lista
t_tcb *list_pop_last_tcb(t_list *list) {
    if (list_size(list) == 0) {
        return NULL; // Si la lista está vacía
    }
    return list_remove(list, list_size(list) - 1); // Remover y retornar el último elemento
}

t_tcb *remove_tcb_from_list(t_list *list, uint32_t pid, uint32_t tid) {
    bool pid_tid_condition(void *tcb) {
        return ((t_tcb *)tcb)->parent_PID == pid && ((t_tcb *)tcb)->TID == tid;
    }
    return list_remove_by_condition(list, pid_tid_condition);
}

t_tcb *find_tcb_in_list(t_list *list, uint32_t pid, uint32_t tid) {
    bool pid_tid_condition(void *tcb) {
        return ((t_tcb *)tcb)->parent_PID == pid && ((t_tcb *)tcb)->TID == tid;
    }
    return list_find(list, pid_tid_condition);
}

int count_tcbs_in_list(t_list *list) {
    return list_size(list);
}

void move_tcb_between_lists(t_list *src_list, t_list *dest_list, uint32_t pid, uint32_t tid, t_state new_state) {
    static const char *state_names[] = {"NEW", "READY", "RUNNING", "BLOCKED", "EXIT"};

    t_tcb *tcb = remove_tcb_from_list(src_list, pid, tid);
    if (tcb != NULL) {
        tcb->state = new_state;
        add_tcb_to_list(dest_list, tcb);
        log_info(logger, "El TCB con PID %d y TID %d fue movido a la lista y su estado fue actualizado a %s",
                 tcb->parent_PID, tcb->TID, state_names[new_state]);
    } else {
        log_warning(logger, "No se encontró un TCB con PID %d y TID %d en la lista de origen.", pid, tid);
    }
}


void log_list_contents_tcb(t_list *list, const char *list_name) {
    log_debug(logger, "\nContenido de la lista %s:", list_name);
    
    for (int i = 0; i < list_size(list); i++) {
        t_tcb *tcb = list_get(list, i);
        log_debug(logger, "TCB PID: %d, TID: %d, Estado: %d, Ruta: %s", tcb->parent_PID, tcb->TID, tcb->state, tcb->file_path);
    }

    log_debug(logger, "Total de TCBs en la lista %s: %d\n", list_name, list_size(list));
}

t_tcb* find_tcb_from_tid(uint32_t tid, uint32_t ppid) {
    t_tcb* tcb = find_tcb_in_list(list_BLOCKED_TCB, ppid, tid);
    
    if (tcb == NULL) {
        tcb = find_tcb_in_list(list_READY_TCB, ppid, tid);
    }
    
    return tcb;
}

int existing_tcb(uint32_t ppid, uint32_t tid) {
    t_tcb* tcb_blocked = find_tcb_in_list(list_BLOCKED_TCB, ppid, tid);
    t_tcb* tcb_ready = find_tcb_in_list(list_READY_TCB, ppid, tid);
    t_tcb* tcb_new = find_tcb_in_list(list_NEW_TCB, ppid, tid);
    
    if(tcb_blocked != NULL || tcb_ready!= NULL || tcb_new != NULL) {
        return 1;
    } else return 0;
}


void block_tcb(t_tcb* tcb) {
    //pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
    add_tcb_to_list(list_BLOCKED_TCB, tcb);
    tcb->state = BLOCKED;
    //pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);
}
