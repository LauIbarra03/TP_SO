#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <pthread.h>
#include <semaphore.h>
#include <utils/estructuras.h>

// Declarar los mutexes globales para las listas
extern pthread_mutex_t mutex_list_NEW_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB;

// Declarar el semáforo para el planificador a largo plazo
extern sem_t sem_long_term_scheduler;

// Funciones para inicializar y destruir los mutexes y el semáforo
void init_scheduler_mutexes();
void destroy_scheduler_mutexes();

t_tcb* schedule_next_tcb();

#endif /* SCHEDULER_H_ */
