#include "mutex_thread.h"
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include <utils/estructuras.h>
#include <tcb_lists.h>
#include <pcb_lists.h>
#include <process.h>
#include <stdbool.h>

extern t_log *logger;
extern t_list *list_BLOCKED_TCB;
extern t_list *list_READY_TCB;
extern t_list *list_EXIT_TCB; // Lista de salida de hilos

extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB; // Mutex para la lista de salida

extern sem_t sem_thread_count;

t_list *global_mutexes = NULL;

void mutex_create(const char *name, t_pcb *pcb)
{
    if (global_mutexes == NULL)
    {
        global_mutexes = list_create();
    }

    if (get_mutex(name) != NULL)
    {
        return;
    }

    t_mutex *new_mutex = malloc(sizeof(t_mutex));
    new_mutex->name = strdup(name);
    new_mutex->assigned_tid = -1;
    new_mutex->assigned_pid = pcb->PID;
    new_mutex->assigned = false;
    new_mutex->blocked_tcbs = list_create();
    new_mutex->blocked_count = 0;

    add_mutex_to_pcb(pcb, new_mutex);

    list_add(global_mutexes, new_mutex);
}

bool mutex_lock(const char *name, t_tcb *tcb)
{
    t_mutex *mutex = get_mutex(name);

    if (mutex == NULL)
    {
        log_error(logger, "El mutex solicitado no existe");
        op_code retorno_delete = delete_thread_memory(tcb);

        if (retorno_delete == OK)
        {
            pthread_mutex_lock(&mutex_list_EXIT_TCB);
            add_tcb_to_list(list_EXIT_TCB, tcb);
            pthread_mutex_unlock(&mutex_list_EXIT_TCB);
        }
        else
        {
            log_info(logger, "No se pudo eliminar el hilo en memoria");
        }
        return false;
    }

    if (mutex->assigned_pid != tcb->parent_PID)
    {
        log_error(logger, "MUTEX PERTENECIENTE A OTRO PROCESO");
        free(mutex);
        return false;
    }

    if (!mutex->assigned || mutex->assigned_tid == tcb->TID)
    {
        mutex->assigned = true;
        mutex->assigned_tid = tcb->TID;
        return true;
    }
    else
    {
        block_tcb_in_mutex_list(tcb, mutex);
        return false;
    }
}

void mutex_unlock(const char *name, t_tcb *tcb)
{
    t_mutex *mutex = get_mutex(name);

    if (mutex == NULL || !mutex->assigned || mutex->assigned_tid != tcb->TID)
    {
        log_error(logger, "ERROR AL EJECUTAR MUTEX_UNLOCK, EL TID DUEÑO ES %d Y EL TID ACTUAL ES %d", mutex->assigned_tid, tcb->TID);
        return;
    }

    t_tcb *proximo_tcb = get_first_blocked(mutex);
    if (proximo_tcb != NULL)
    {
        mutex->assigned_tid = proximo_tcb->TID;

        pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
        pthread_mutex_lock(&mutex_list_READY_TCB);

        move_tcb_between_lists(list_BLOCKED_TCB, list_READY_TCB, proximo_tcb->parent_PID, proximo_tcb->TID, READY);
        list_remove(mutex->blocked_tcbs, 0); // Elimina el primer elemento
        mutex->blocked_count--;

        sem_post(&sem_thread_count);

        pthread_mutex_unlock(&mutex_list_READY_TCB);
        pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);
    }
    else
    {
        mutex->assigned = false;
        mutex->assigned_tid = -1;
    }
}

void release_all_mutexes(t_tcb *tcb)
{
    if (global_mutexes != NULL)
    {
        for (int i = 0; i < list_size(global_mutexes); i++)
        {
            t_mutex *mutex = list_get(global_mutexes, i);
            if (mutex->assigned && mutex->assigned_tid == tcb->TID)
            {
                log_info(logger, "Liberando mutex '%s' asociado al TCB con TID %d", mutex->name, tcb->TID);
                mutex_unlock(mutex->name, tcb);
            }
        }
    }
}

t_mutex *get_mutex(const char *name)
{
    if (global_mutexes != NULL)
    {

        for (int i = 0; i < list_size(global_mutexes); i++)
        {
            t_mutex *mutex = list_get(global_mutexes, i);
            if (strcmp(mutex->name, name) == 0)
            {
                return mutex;
            }
        }
        log_warning(logger, "El mutex encontrado de get_mutex es NULL");
    }
    log_info(logger, "La lista global de mutexes es null. Abortando get_mutex");
    return NULL;
}

t_tcb *get_first_blocked(t_mutex *mutex)
{
    if (list_is_empty(mutex->blocked_tcbs))
    {
        log_warning(logger, "No hay tcbs bloqueados");
        return NULL;
    }

    t_tcb *tcb = list_get(mutex->blocked_tcbs, 0);
    return tcb;
}


void block_tcb_in_mutex_list(t_tcb *tcb, t_mutex *mutex)
{
    pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
    tcb->state = BLOCKED;
    add_tcb_to_list(list_BLOCKED_TCB, tcb);
    pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);
    log_info(logger, "## (PID:TID) : (%i:%i) - Bloqueado por: MUTEX", tcb->parent_PID, tcb->TID);
    list_add(mutex->blocked_tcbs, tcb);
    mutex->blocked_count++;
    log_info(logger, "TCB con TID %d bloqueado en mutex '%s'", tcb->TID, mutex->name);
}

void free_mutex(t_mutex *mutex)
{
    if (mutex == NULL)
    {
        return;
    }

    if (mutex->name != NULL)
    {
        free(mutex->name);
    }

    if (mutex->blocked_tcbs != NULL)
    {
        for (uint32_t i = 0; i < mutex->blocked_count; i++)
        {
            delete_tcb(mutex->blocked_tcbs[i]);
        }

        list_destroy(mutex->blocked_tcbs);
    }

    free(mutex);
}

void add_mutex_to_pcb(t_pcb *pcb, t_mutex *mutex)
{
    pcb->mutex_count++;
    pcb->mutex = realloc(pcb->mutex, pcb->mutex_count * sizeof(t_mutex *));

    if (!pcb->mutex)
    {
        log_error(logger, "Error al reasignar memoria para el array de mutexes en el PCB");
        exit(EXIT_FAILURE);
    }

    pcb->mutex[pcb->mutex_count - 1] = mutex;
}

/*void init_mutex_waitlist() {
    mutex_waitlist = list_create();
    if (pthread_mutex_init(&mutex_waitlist_lock, NULL) != 0) {
        log_error(logger, "Error al inicializar mutex_waitlist_lock");
    } else {
        log_info(logger, "mutex_waitlist y mutex_waitlist_lock inicializados correctamente");
    }
}

void add_mutex_request(t_mutex_request* request) {
    // Proteger la lista de espera de mutexes con el mutex de protección
    pthread_mutex_lock(&mutex_waitlist_lock);
    list_add(mutex_waitlist, request);
    pthread_mutex_unlock(&mutex_waitlist_lock);

    log_info(logger, "Solicitud de mutex añadida a la lista de espera: PID %d, TID %d, Recurso %s",
             request->PID, request->TID, request->resource_name);
}

// Función para solicitar un mutex
void request_mutex(t_tcb *tcb, char *resource_name)
{
    // Obtener el proceso padre usando el PID
    t_pcb *parent_pcb = find_pcb_by_pid(tcb->parent_PID);
    if (parent_pcb == NULL) {
        log_error(logger, "No se encontró el proceso padre con PID %d", tcb->parent_PID);
        return;
    }

    // Buscar si el mutex está ya asignado a otro TCB
    bool is_mutex_assigned(void *mutex_element)
    {
        t_mutex_pcb *mutex = (t_mutex_pcb *)mutex_element;
        return strcmp(mutex->name, resource_name) == 0 && mutex->assigned;
    }

    t_mutex_pcb *mutex = list_find(parent_pcb->mutex, is_mutex_assigned);

    if (mutex == NULL || !mutex->assigned)
    {
        // Si no está asignado, asignar el mutex al TCB actual
        mutex->assigned = true;
        mutex->assigned_tid = tcb->TID;
        log_info(logger, "Mutex %s asignado a TID %d (PID %d)", resource_name, tcb->TID, tcb->parent_PID);
    }
    else
    {
        // Si el mutex está ocupado, bloquear el TCB y añadir la solicitud a la lista de espera
        tcb->state = BLOCKED;
        pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
        add_tcb_to_list(list_BLOCKED_TCB, tcb);
        pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

        t_mutex_request *request = malloc(sizeof(t_mutex_request));
        request->PID = tcb->parent_PID;
        request->TID = tcb->TID;
        request->resource_name = strdup(resource_name);

        add_mutex_request(request);

        log_info(logger, "TID %d (PID %d) bloqueado esperando el mutex %s", tcb->TID, tcb->parent_PID, resource_name);
    }
}

// Función para liberar un mutex
void release_mutex(t_tcb *tcb, char *resource_name)
{
    // Obtener el proceso padre usando el PID
    t_pcb *parent_pcb = find_pcb_by_pid(tcb->parent_PID);
    if (parent_pcb == NULL) {
        log_error(logger, "No se encontró el proceso padre con PID %d", tcb->parent_PID);
        return;
    }

    // Buscar el mutex correspondiente al nombre de recurso
    bool is_mutex_to_release(void *mutex_element)
    {
        t_mutex_pcb *mutex = (t_mutex_pcb *)mutex_element;
        return strcmp(mutex->name, resource_name) == 0 && mutex->assigned_tid == tcb->TID;
    }

    t_mutex_pcb *mutex = list_find(parent_pcb->mutex, is_mutex_to_release);

    if (mutex != NULL)
    {
        mutex->assigned = false;
        mutex->assigned_tid = -1;
        log_info(logger, "Mutex %s liberado por TID %d (PID %d)", resource_name, tcb->TID, tcb->parent_PID);

        // Revisar si hay solicitudes en espera para el mutex liberado
        bool is_waiting_for_mutex(void *request_element)
        {
            t_mutex_request *request = (t_mutex_request *)request_element;
            return strcmp(request->resource_name, resource_name) == 0;
        }

        pthread_mutex_lock(&mutex_waitlist_lock);
        t_mutex_request *next_request = list_remove_by_condition(mutex_waitlist, is_waiting_for_mutex);
        pthread_mutex_unlock(&mutex_waitlist_lock);

        if (next_request != NULL)
        {
            // Mover el TCB de la lista BLOCKED a READY
            pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
            t_tcb *waiting_tcb = find_tcb_in_list(list_BLOCKED_TCB, next_request->PID, next_request->TID);
            if (waiting_tcb != NULL)
            {
                waiting_tcb->state = READY;
                move_tcb_between_lists(list_BLOCKED_TCB, list_READY_TCB, next_request->PID, next_request->TID, READY);
                log_info(logger, "TID %d (PID %d) movido de BLOCKED a READY al liberar el mutex %s", next_request->TID, next_request->PID, resource_name);
            }
            pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

            free(next_request->resource_name);
            free(next_request);
        }
    }
    else
    {
        log_warning(logger, "El mutex %s no estaba asignado al TID %d (PID %d)", resource_name, tcb->TID, tcb->parent_PID);
    }
} */
