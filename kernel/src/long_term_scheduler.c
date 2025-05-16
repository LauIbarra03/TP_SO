// long_term_scheduler.c

#include "long_term_scheduler.h"
#include "scheduler.h"
#include <commons/collections/queue.h>
#include <utils/tcb_serialization.h>
#include <utils/estructuras.h>
#include <semaphore.h>

extern t_log* logger;
sem_t sem_long_term_scheduler;

// Inicialización del semáforo del Planificador de Largo Plazo
/*
void init_long_term_scheduler() {
    sem_init(&sem_long_term_scheduler, 0, 0);  // Inicializamos el semáforo en 0
}
*/

// Función que correrá en un hilo separado y que se encargará de manejar el Planificador de Largo Plazo
void* long_term_scheduler_routine(void* arg) {

    log_info(logger, "Inicializando long term scheduler");

    while (1) {
        // 1. Esperamos a que haya hilos en la lista de NEW
        sem_wait(&sem_long_term_scheduler);

        // 2. Bloquear acceso a la lista de NEW
        pthread_mutex_lock(&mutex_list_NEW_TCB);

        // 3. Hacemos un pop de la cola de NEW
        t_tcb* tcb = queue_pop(list_NEW_TCB);

        // Desbloqueamos la lista de NEW
        pthread_mutex_unlock(&mutex_list_NEW_TCB);

        if (tcb == NULL) {
            continue;  // Si no se encontró un TCB, volvemos a empezar el loop
        }

        // 4. Aquí irá la comunicación con Memoria (futuro espacio)
        // TODO: Comunicar con Memoria

        // 5. Cambiamos el estado del TCB de NEW a READY
        tcb->state = READY;

        // 6. Bloquear acceso a la lista de READY
        pthread_mutex_lock(&mutex_list_READY_TCB);

        // Añadir el TCB a la cola de READY
        queue_push(list_READY_TCB, tcb);

        // Desbloquear la lista de READY
        pthread_mutex_unlock(&mutex_list_READY_TCB);

        // 7. Loggear la admisión del TCB a READY
        log_info(logger, "TCB con PID %d y TID %d ha sido admitido a READY", tcb->parent_PID, tcb->TID);
    }
    return NULL;
}
