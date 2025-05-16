#ifndef LONG_TERM_SCHEDULER_H_
#define LONG_TERM_SCHEDULER_H_

#include <semaphore.h>
#include <pthread.h>
#include <commons/log.h>
#include "tcb_lists.h"

// Declaración del semáforo del Planificador de Largo Plazo
extern sem_t sem_long_term_scheduler;

// Inicialización del Planificador de Largo Plazo
//void init_long_term_scheduler();

// Función del Planificador de Largo Plazo que correrá en un hilo separado
void* long_term_scheduler_routine(void* arg);

#endif /* LONG_TERM_SCHEDULER_H_ */
