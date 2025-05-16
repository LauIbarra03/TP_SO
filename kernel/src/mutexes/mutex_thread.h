#ifndef MUTEX_THREAD_H_
#define MUTEX_THREAD_H_

#include <pthread.h>
#include <commons/collections/list.h>
#include <stdint.h>
#include <utils/estructuras.h>  // Incluir el archivo donde está definido t_tcb

// Estructura para solicitudes de mutex en espera
typedef struct {
    char *name;                 // Nombre único del mutex
    uint32_t assigned_pid;      // ID del proceso al que pertenece el mutex
    uint32_t assigned_tid;      // ID del hilo al que está asignado (0 si no está asignado)
    bool assigned;              // Indica si el mutex está asignado o no
    t_tcb **blocked_tcbs;       // Lista de TCBS bloqueados esperando el mutex
    uint32_t blocked_count;     // Cantidad de hilos bloqueados
} t_mutex;

extern t_list *global_mutexes;

/**
 * Crea un mutex con el nombre dado y lo asigna al PCB correspondiente.
 * Si ya existe un mutex con el mismo nombre, no realiza ninguna acción.
 */
void mutex_create(const char* name, t_pcb* pcb);

/**
 * Intenta adquirir un mutex para el TCB proporcionado.
 * Si el mutex está disponible, lo asigna al TCB.
 * Si no está disponible, bloquea al TCB en la lista de espera del mutex.
 * Retorna false si el proceso debe ser desalojado, true si la reserva fue exitosa y no se debe desalojar
 */
bool mutex_lock(const char *name, t_tcb* tcb);

/**
 * Libera un mutex asignado al TCB proporcionado.
 * Si hay hilos esperando por el mutex, el primero en bloquearse es movido a READY y adquiere el mutex.
 */
void mutex_unlock(const char *name, t_tcb* tcb);

/**
 * Busca y retorna un mutex por su nombre.
 * Si no se encuentra el mutex, retorna NULL.
 */
t_mutex *get_mutex(const char *name);

/**
 * Retorna el primer TCB bloqueado en la cola de espera del mutex.
 * Si no hay hilos bloqueados, retorna NULL.
 */
t_tcb* get_first_blocked(t_mutex* mutex);

/**
 * Bloquea al TCB proporcionado y lo añade a la lista de espera del mutex especificado.
 * También mueve al TCB a la lista de TCBS bloqueados global.
 */
void block_tcb_in_mutex_list(t_tcb* tcb, t_mutex* mutex);

/**
 * Libera la memoria asociada al mutex, incluyendo su nombre y la lista de TCBS bloqueados.
 */
void free_mutex(t_mutex *mutex);

/**
 * Añade un mutex al PCB especificado, actualizando el array de mutexes en el PCB.
 */
void add_mutex_to_pcb(t_pcb* pcb, t_mutex* mutex);

/**
 * Libera todos los mutexes asignados a un TCB y desbloquea los hilos bloqueados correspondientes.
 */
void release_all_mutexes(t_tcb* tcb);












/*typedef struct {
    uint32_t PID;          // ID del proceso que solicita el mutex
    uint32_t TID;          // ID del hilo que solicita el mutex
    char* resource_name;   // Nombre del recurso (mutex) solicitado
} t_mutex_request; 

// Lista de espera de solicitudes de mutex y el mutex de protección
extern t_list* mutex_waitlist;
extern pthread_mutex_t mutex_waitlist_lock;

/**
 * @brief Inicializa la lista de espera de mutexes y el mutex de protección.
 */
//void init_mutex_waitlist();

/**
 * @brief Agrega una nueva solicitud de mutex a la lista de espera.
 *
 * @param request Puntero a la solicitud de mutex que se añadirá a la lista.
 */
//void add_mutex_request(t_mutex_request* request);

/**
 * @brief Solicita el uso de un mutex. Si el mutex está ocupado, añade la solicitud a la lista de espera.
 *
 * @param tcb Puntero al TCB del hilo que solicita el mutex.
 * @param resource_name Nombre del recurso solicitado.
 */
//void request_mutex(t_tcb *tcb, char *resource_name);

/**
 * @brief Libera un mutex. Si hay solicitudes en espera para el mutex, asigna el recurso al siguiente en la lista.
 *
 * @param tcb Puntero al TCB del hilo que libera el mutex.
 * @param resource_name Nombre del recurso a liberar.
 */
//void release_mutex(t_tcb *tcb, char *resource_name);

/**
 * @brief Elimina una solicitud específica de la lista de espera de mutexes.
 *
 * @param pid ID del proceso asociado a la solicitud.
 * @param tid ID del hilo asociado a la solicitud.
 * @param resource_name Nombre del recurso solicitado.
 */
//void release_mutex_request(uint32_t pid, uint32_t tid, char *resource_name); */

#endif /* MUTEX_THREAD_H_ */
