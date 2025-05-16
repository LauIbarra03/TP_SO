#include "quantum_timer.h"
#include <kernel_interrupt/kernel_interrupt.h>
#include <commons/temporal.h>
#include <commons/log.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

extern t_log *logger;  // Uso del logger externamente
extern sem_t sincro_interrupcion;

// Definir los semáforos y mutexes
sem_t sem_quantum;
sem_t sem_quantum_finished;
pthread_mutex_t mutex_quantum_interrupted;
bool quantum_interrupted;


// Inicializar las variables del quantum timer
void init_quantum_timer_variables() {
    if (sem_init(&sem_quantum, 0, 0) != 0) {
        log_error(logger, "Error al inicializar semáforo sem_quantum");
    } else {
        //log_info(logger, "Semáforo sem_quantum inicializado correctamente");
    }

    if (sem_init(&sem_quantum_finished, 0, 0) != 0) {
        log_error(logger, "Error al inicializar semáforo sem_quantum_finished");
    } else {
        //log_info(logger, "Semáforo sem_quantum_finished inicializado correctamente");
    }

    // Inicializar el mutex
    if (pthread_mutex_init(&mutex_quantum_interrupted, NULL) != 0) {
        log_error(logger, "Error al inicializar mutex mutex_quantum_interrupted");
    } else {
        //log_info(logger, "Mutex mutex_quantum_interrupted inicializado correctamente");
    }

    quantum_interrupted = false;
}

// Función para el hilo de quantum
void* run_quantum_counter(void *arg) {
    int quantum_time = *((int*) arg); 

    //log_info(logger, " ");
    log_info(logger, "Hilo de Quantum inicializado con tiempo de quantum: %d ms", quantum_time);
    log_info(logger, "Esperando llamado a inicio de quantum\n");

    while (1) {
        sem_wait(&sem_quantum);
        log_info(logger, "Comenzando ráfaga de quantum de %d ms", quantum_time);

        t_temporal *timer = temporal_create();
        temporal_resume(timer);

        pthread_mutex_lock(&mutex_quantum_interrupted);
        quantum_interrupted = false;  // Reiniciar la variable de interrupción
        pthread_mutex_unlock(&mutex_quantum_interrupted);

        while (temporal_gettime(timer) < quantum_time && !quantum_interrupted) {
            pthread_mutex_lock(&mutex_quantum_interrupted);
            if (quantum_interrupted) {
                pthread_mutex_unlock(&mutex_quantum_interrupted);
                break;
            }
            pthread_mutex_unlock(&mutex_quantum_interrupted);
            usleep(1000);
        }

        temporal_stop(timer);
        temporal_destroy(timer);

        if (!quantum_interrupted) {
            log_info(logger, "Quantum completado");
            //sem_post(&sem_quantum_finished);  // Señalar que el quantum ha terminado
            sem_wait(&mutex_quantum_interruption);
            quantum_interruption = 1;           // Señalar a la variable global quantum_interruption
            sem_post(&sincro_interrupcion);
            sem_post(&mutex_quantum_interruption);
        } else {
            log_info(logger, "Quantum interrumpido");
        }
    }

    return NULL;
}

// Función para interrumpir el quantum
void interrupt_quantum() {
    pthread_mutex_lock(&mutex_quantum_interrupted);
    quantum_interrupted = true;  // Setear la interrupción
    pthread_mutex_unlock(&mutex_quantum_interrupted);
    //log_info(logger, "Quantum interrumpido");
}

// Función para iniciar el quantum
void start_quantum() {
    sem_post(&sem_quantum);  // Hacer un post para iniciar el quantum
    log_info(logger, "Quantum iniciado");
}

// Función para detener el quantum
void stop_quantum() {
    //sem_wait(&sem_quantum);  // Esperar el semáforo para detener el quantum
    interrupt_quantum();      // Interrumpir el quantum al detener
    log_info(logger, "Quantum interrumpido");
}
