//scheduler.c

#include "scheduler.h"
#include <commons/log.h>
#include <semaphore.h>
#include <stdio.h>
#include <utils/estructuras.h>
#include <tcb_lists.h>
#include <quantum_timer.h>

// Definir los mutexes globales
pthread_mutex_t mutex_list_NEW_TCB;
pthread_mutex_t mutex_list_READY_TCB;
pthread_mutex_t mutex_list_BLOCKED_TCB;
pthread_mutex_t mutex_list_EXIT_TCB;

extern sem_t sem_long_term_scheduler;
extern t_log* logger;
extern char* scheduler_algorithm;
extern int scheduler_quantum;

// Inicializar los mutexes
void init_scheduler_mutexes() {
    if (pthread_mutex_init(&mutex_list_NEW_TCB, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex de la lista NEW_TCB");
    }
    if (pthread_mutex_init(&mutex_list_READY_TCB, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex de la lista READY_TCB");
    }
    if (pthread_mutex_init(&mutex_list_BLOCKED_TCB, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex de la lista BLOCKED_TCB");
    }
    if (pthread_mutex_init(&mutex_list_EXIT_TCB, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex de la lista EXIT_TCB");
    }

    log_info(logger, "Mutexes del planificador inicializados correctamente");
}

// Destruir los mutexes
void destroy_scheduler_mutexes() {
    pthread_mutex_destroy(&mutex_list_NEW_TCB);
    pthread_mutex_destroy(&mutex_list_READY_TCB);
    pthread_mutex_destroy(&mutex_list_BLOCKED_TCB);
    pthread_mutex_destroy(&mutex_list_EXIT_TCB);
    log_info(logger, "Mutexes del planificador destruidos correctamente");
}

t_tcb* schedule_next_FIFO() {
    
    if(list_READY_TCB->elements_count <= 0){
        return NULL;
    }
    pthread_mutex_lock(&mutex_list_READY_TCB);
    t_tcb * tcb = list_pop_first_tcb(list_READY_TCB);
    pthread_mutex_unlock(&mutex_list_READY_TCB);

    return tcb;
}

t_tcb* schedule_next_PRIORITIES() {
    pthread_mutex_lock(&mutex_list_READY_TCB);
    
    if (list_is_empty(list_READY_TCB)) {
        pthread_mutex_unlock(&mutex_list_READY_TCB);
        return NULL;
    }

    // Buscamos el TCB con la prioridad más baja
    t_tcb* selected_tcb = list_get(list_READY_TCB, 0);
    int selected_index = 0;

    for (int i = 1; i < list_size(list_READY_TCB); i++) {
        t_tcb* current_tcb = list_get(list_READY_TCB, i);
        if (current_tcb->priority < selected_tcb->priority) {
            selected_tcb = current_tcb;
            selected_index = i;
        }
    }

    // Eliminamos el TCB seleccionado de la lista
    selected_tcb = list_remove(list_READY_TCB, selected_index);

    pthread_mutex_unlock(&mutex_list_READY_TCB);

    return selected_tcb;
}


t_tcb* schedule_next_tcb() {
    t_tcb* next_pcb;

    if (strcmp(scheduler_algorithm, "FIFO") == 0) {

        next_pcb = schedule_next_FIFO();

    } else if (strcmp(scheduler_algorithm, "PRIORIDADES") == 0) {

        next_pcb = schedule_next_PRIORITIES();

    } else if (strcmp(scheduler_algorithm, "CMN") == 0) {

        next_pcb = schedule_next_PRIORITIES();
        start_quantum();

    } else {
        log_error(logger, "Algoritmo de planificación desconocido: %s\n", scheduler_algorithm);
        return NULL;
    }

    return next_pcb;
}

