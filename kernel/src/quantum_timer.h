#ifndef QUANTUM_TIMER_H_
#define QUANTUM_TIMER_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

// Definición de semáforos y mutexes
extern sem_t sem_quantum;
extern sem_t sem_quantum_finished;
extern pthread_mutex_t mutex_quantum_interrupted;
extern bool quantum_interrupted;  // Variable global para la interrupción

// Funciones de inicialización y ejecución
void init_quantum_timer_variables();
void* run_quantum_counter(void *arg);

// Función para interrumpir la ejecución de quantum
// Si la ejecución está interrumpida, el fin de quantum no hace nada
void interrupt_quantum();

// Función para iniciar el quantum (post al semáforo)
void start_quantum();

// Función para detener el quantum (wait al semáforo y setear interrupción)
void stop_quantum();

#endif /* QUANTUM_TIMER_H_ */
