#include <syscalls_operations/memory_operations.h>

extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB;

extern t_list *list_READY_TCB;
extern t_list *list_BLOCKED_TCB;
extern t_list *list_EXIT_TCB;

extern sem_t sem_thread_count;

void dump_memory_thread(void *tcb_sin_castear)
{
    t_tcb *tcb = (t_tcb*)tcb_sin_castear;

    log_debug(logger, "tcb del dump PID: %d, TID: %d", tcb->parent_PID, tcb->TID);

    t_dump_memory *dump_memory = malloc(sizeof(t_dump_memory));
    dump_memory->pid = tcb->parent_PID;
    dump_memory->tid = tcb->TID;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_dump_memory(dump_memory, buffer);

    t_paquete *paquete = create_package(DUMP_MEMORY);

    add_to_package(paquete, buffer->stream, buffer->size);

    int memory_kernel_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");

    if (memory_kernel_connection == -1)
    {
        log_error(logger, "Error al conectar con Memoria");
        free(buffer->stream);
        free(buffer);
        delete_package(paquete);
        return ERROR;
    }

    send_package(paquete, memory_kernel_connection);

    int response = wait_for_response(memory_kernel_connection);

    log_warning(logger, "Respuesta recibida de memoria: %d", response);

    send_response(memory_kernel_connection, DISCONNECT);

    destroy_dump_memory(dump_memory);
    delete_package(paquete);
    free(buffer->stream);
    free(buffer);
    free_connection(memory_kernel_connection);

    if (response == OK)
    {
        log_info(logger, "DUMP memory exitoso");
        
        // Mover el TCB bloqueado de BLOCKED a READY
        pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
        pthread_mutex_lock(&mutex_list_READY_TCB);
        move_tcb_between_lists(list_BLOCKED_TCB, list_READY_TCB, tcb->parent_PID, tcb->TID, READY);
        pthread_mutex_unlock(&mutex_list_READY_TCB);
        pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

        log_info(logger, "El hilo (PID:TID) (%d:%d) se desbloqueó posterior al DUMP", tcb->parent_PID, tcb->TID);

        sem_post(&sem_thread_count);
    }
    else
    {
        log_info(logger, "DUMP memory sin exito");

        t_pcb* pcb = find_pcb_by_pid(tcb->parent_PID);
        
        for (uint32_t i = 0; i < pcb->tid_count; i++)
            {
                t_tcb *tcb = &(pcb->tcbs[i]); // Obtener el TCB

                // Enviar solicitud de eliminación de hilo a Memoria
                delete_thread_memory(tcb);

                if (pcb->tid_count > 1)
                {
                    // if el num de tcbs es mayor a 0 hacer este wait
                    sem_wait(&sem_thread_count);
                }

                // Mover el TCB a la lista de salida
                add_tcb_to_list(list_EXIT_TCB, tcb);

                // Desbloquear cualquier hilo esperando en un join
                desbloquear_thread_por_join(tcb->TID);

                // Liberar todos los mutexes asociados al tcb
                release_all_mutexes(tcb);
            }

            delete_process_memory(pcb);

            remove_pcb_from_active(pcb->PID);
        
        log_info(logger, "El proceso (PID:TID) (%d:%d) se envió a exit por una falla en su memory dump", tcb->parent_PID, tcb->TID);
    }
    delete_tcb(tcb);
}