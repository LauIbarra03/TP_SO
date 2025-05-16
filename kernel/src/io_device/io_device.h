#ifndef IO_DEVICE_H_
#define IO_DEVICE_H_

#include <pthread.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>
#include <stdint.h>
#include <utils/estructuras.h>
#include <semaphore.h>

// Estructura para las solicitudes de IO
typedef struct {
    uint32_t io_time;       // Tiempo de simulación en milisegundos
    uint32_t PID;           // PID del proceso
    uint32_t TID;           // TID del hilo
} io_request;

extern t_queue *io_request_queue;
extern pthread_mutex_t mutex_io_request_queue;
extern sem_t sem_io_request;

/**
 * @brief Inicializa la cola de solicitudes de IO, el mutex y el semáforo asociado.
 */
void init_io_queue();

/**
 * @brief Libera los recursos asociados a la cola de IO, el mutex y el semáforo.
 */
void destroy_io_queue();

/**
 * @brief Función para generar una nueva solicitud de IO.
 * @param tcb Puntero al TCB del hilo que solicita IO.
 * @param io_time Tiempo de IO en milisegundos.
 */
void request_io(t_tcb *tcb, uint32_t io_time);

/**
 * @brief Hilo que maneja las solicitudes de IO, procesando cada solicitud
 * y moviendo el TCB de BLOCKED a READY una vez completado el tiempo de IO.
 */
void *io_device_thread();

#endif /* IO_DEVICE_H_ */
