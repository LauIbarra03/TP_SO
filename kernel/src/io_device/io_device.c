// io_device.c

#include "io_device.h"
#include <stdlib.h>
#include <commons/log.h>
#include "tcb_lists.h"
#include <semaphore.h>
#include <unistd.h>  // Para usleep()

// Definir la cola, el mutex y el semáforo
t_queue *io_request_queue = NULL;
pthread_mutex_t mutex_io_request_queue;
sem_t sem_io_request;  // Semáforo para IO

extern t_log *logger;  // Usar el logger externamente

extern sem_t sem_thread_count;

// Inicializa la cola, el mutex y el semáforo de IO
void init_io_queue() {
    io_request_queue = queue_create();
    pthread_mutex_init(&mutex_io_request_queue, NULL);
    sem_init(&sem_io_request, 0, 0);  // Inicializa el semáforo con valor 0
    log_info(logger, "Cola, mutex y semáforo de IO inicializados.");
}

// Libera los recursos de la cola, el mutex y el semáforo de IO
void destroy_io_queue() {
    queue_destroy_and_destroy_elements(io_request_queue, free); // Libera las solicitudes de IO
    pthread_mutex_destroy(&mutex_io_request_queue);
    sem_destroy(&sem_io_request);  // Destruye el semáforo
    log_info(logger, "Cola, mutex y semáforo de IO destruidos.");
}

// Genera una nueva solicitud de IO y la añade a la cola
void request_io(t_tcb *tcb, uint32_t io_time) {
    // Crear una nueva solicitud de IO
    io_request *request = malloc(sizeof(io_request));
    if (request == NULL) {
        log_error(logger, "Error al asignar memoria para la solicitud de IO.");
        return;
    }

    request->io_time = io_time;
    request->PID = tcb->parent_PID;
    request->TID = tcb->TID;

    // Bloquear el acceso a la cola de IO y añadir la solicitud
    pthread_mutex_lock(&mutex_io_request_queue);
    queue_push(io_request_queue, request);
    pthread_mutex_unlock(&mutex_io_request_queue);

    // Incrementar el valor del semáforo en 1
    sem_post(&sem_io_request);

    // log_info(logger, "Solicitud de IO creada para el TID %d del PID %d con tiempo de %d ms.", request->TID, request->PID, request->io_time);

    // Cambiar el estado del TCB a BLOCKED
    tcb->state = BLOCKED;

    // Bloquear la lista de BLOCKED y añadir el TCB a la lista
    pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
    log_info(logger, "## (PID:TID) : (%i:%i) Bloqueado por: IO %d", tcb->parent_PID, tcb->TID, request->io_time);
    add_tcb_to_list(list_BLOCKED_TCB, tcb);
    pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

    // log_info(logger, "El TCB con PID %d y TID %d fue movido a la lista BLOCKED.", tcb->parent_PID, tcb->TID);
}

// Función que manejará el hilo de IO
void *io_device_thread() {
    log_info(logger, "Dispositivo de IO inicializado");
    while (1) {
        // 1. Esperar por una solicitud de IO (se bloquea si la cola está vacía)
        sem_wait(&sem_io_request);

        // 2. Bloquear acceso a la cola de IO y extraer el primer request
        pthread_mutex_lock(&mutex_io_request_queue);
        io_request *request = queue_pop(io_request_queue);
        pthread_mutex_unlock(&mutex_io_request_queue);

        if (request == NULL) {
            log_error(logger, "Error: se intentó procesar un request IO nulo.");
            continue;
        }

        // 3. Esperar el tiempo de IO en milisegundos
        log_info(logger, "## (PID:TID) : (%i:%i) inicio IO (%i)", request->PID, request->TID, request->io_time);

        usleep(request->io_time * 1000);  // usleep recibe microsegundos, por lo que multiplicamos por 1000

        log_info(logger, "## (PID:TID) : (%i:%i) finalizó IO (%i) y pasa a READY", request->PID, request->TID, request->io_time);

        // 4. Extraer el TCB correspondiente de la lista de BLOCKED
        pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
        t_tcb *tcb = remove_tcb_from_list(list_BLOCKED_TCB, request->PID, request->TID);
        pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

        if (tcb == NULL) {
            log_error(logger, "Error: no se encontró el TCB para PID: %d, TID: %d en la lista de BLOCKED", request->PID, request->TID);
            free(request);  // Liberar el request ya que no es útil
            continue;
        }

        // 5. Cambiar el estado del TCB a READY
        tcb->state = READY;

        // 6. Mover el TCB a la lista de READY
        pthread_mutex_lock(&mutex_list_READY_TCB);
        add_tcb_to_list(list_READY_TCB, tcb);
        pthread_mutex_unlock(&mutex_list_READY_TCB);

        // log_info(logger, "El TCB con PID: %d, TID: %d se movió de BLOCKED a READY", request->PID, request->TID);
        sem_post(&sem_thread_count);
        // Liberar el request después de procesarlo
        free(request);
    }

    return NULL;
}
