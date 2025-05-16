#include "kernel_dispatch.h"
#include <tcb_lists.h>
#include <quantum_timer.h>
#include <semaphore.h>
#include <join_thread.h>
#include <mutexes/mutex_thread.h>
#include <long_term_list.h>

extern sem_t sem_thread_count;

extern t_list *list_first_thread_params;

void run_server_dispatch(void *arg)
{
    char *puerto_server = (char *)arg;

    server_fd = start_server(puerto_server);
    log_info(logger, "Server ready to receive clients...");

    while (1)
    {
        int cliente_fd = wait_for_client(server_fd);

        pthread_t client_thread;
        socket_server = malloc(sizeof(int));
        *socket_server = cliente_fd;

        if (pthread_create(&(client_thread), NULL, (void *)enviar_pcb_y_esperar_respuesta, (void *)socket_server) != 0)
        {
            log_error(logger, "Error creating thread for the client.");
            free(socket_server);
            free_connection(cliente_fd);
            exit(0);
        }
        pthread_join(client_thread, NULL);
    }
}

void message_sending_example(int socket)
{
    const char *text = "Hello from CPU!!";
    int size = strlen(text) + 1;

    t_message *message = create_message(text, size);

    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_message(message, buffer);

    t_paquete *package_hello = create_package(MESSAGE);
    add_to_package(package_hello, buffer->stream, buffer->size);

    send_package(package_hello, socket);
    delete_package(package_hello);
}

void enviar_pcb_y_esperar_respuesta(void *arg)
{
    t_config *config = (t_config *)arg;

    bool seguir_ejecutando_mismo_tcb = false;
    bool mandar_tcb_previo_a_ready = false;
    t_tcb *previo_tcb = NULL;

    /*char *ip_cpu = config_get_string_value(config, "IP_CPU");
    char *puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");

    int server_cpu_dispatch_fd = create_connection(ip_cpu, puerto_cpu_dispatch); */

    int server_cpu_dispatch_fd = connection_by_config(config, "IP_CPU", "PUERTO_CPU_DISPATCH");

    if (server_cpu_dispatch_fd < 0)
    {
        log_error(logger, "Invalid Client! Closing Connection. Kernel could not create connection with cpu.");
        free_connection(server_cpu_dispatch_fd);
        // free(ip_cpu);
        // free(puerto_cpu_dispatch);
        return;
    }

    // free(ip_cpu);
    // free(puerto_cpu_dispatch);
    t_tcb *proximo_tcb = NULL;
    log_info(logger, "New client connected, socket fd: %d", server_cpu_dispatch_fd);
    while (1)
    {

        if (seguir_ejecutando_mismo_tcb)
        {
            proximo_tcb = previo_tcb;
        }
        else
        {
            log_info(logger, "Esperando hilo en ready");
            sem_wait(&sem_thread_count);

            proximo_tcb = schedule_next_tcb();

            log_debug(logger, "Se está ejecutando el tcb con TID: %d y PPID: %d", proximo_tcb->TID, proximo_tcb->parent_PID);
        }

        seguir_ejecutando_mismo_tcb = true;
        mandar_tcb_previo_a_ready = false;

        t_pcb *nuevo_pcb_ejecutando = find_pcb_by_pid(proximo_tcb->parent_PID);

        if (nuevo_pcb_ejecutando == NULL)
        {
            log_error(logger, "********************EL NUEVO PCB EJECUTANDO ES NULL********************");
        }

        if (proximo_tcb == NULL)
        {
            log_error(logger, "********************EL PROXIMO TCB ES NULL********************");
        }

        // Poner mutex aca
        current_pcb_executing = nuevo_pcb_ejecutando;

        dispatch_thread(proximo_tcb, server_cpu_dispatch_fd);

        int size;
        int cod_op = receive_operation_code(server_cpu_dispatch_fd);
        void *buffer = receive_buffer(&size, server_cpu_dispatch_fd);
        interrupt_quantum();

        int sem_value; // This is an integer
        sem_getvalue(&sem_thread_count, &sem_value);
        // log_warning(logger, "\n\n\nPID: %d, TID: %d, el semaforo es %d", current_pcb_executing->PID, proximo_tcb->TID, sem_value);

        switch (cod_op)
        {
        case PROCESS_CREATE:
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: PROCESS_CREATE", current_pcb_executing->PID, proximo_tcb->TID);
            t_process_create *proceso_create = deserialize_process_create(buffer);
            // log_info(logger, "el archivo de pseudocodigo es: %s\n", proceso_create->nombre_archivo);
            // log_info(logger, "el tamanio del proceso es: %d\n", proceso_create->tamanio_proceso);
            // log_info(logger, "la prioridad es: %d \n", proceso_create->prioridad);

            op_code res_mem = create_process(proceso_create->tamanio_proceso, proceso_create->nombre_archivo, proceso_create->prioridad);
            list_add(list_first_thread_params, proceso_create);

            try_create_NEW_process_loop();

            break;

        case PROCESS_EXIT:
            stop_quantum();
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: PROCESS_EXIT", current_pcb_executing->PID, proximo_tcb->TID);

            t_process_exit *proceso_exit = deserialize_process_exit(buffer);

            // log_info(logger, "El pid correspondiente es: %d", proceso_exit->pid);
            // log_info(logger, "El pid del proceso actual es %d, su tid_count es %d\n", nuevo_pcb_ejecutando->PID, nuevo_pcb_ejecutando->tid_count);

            for (uint32_t i = 0; i < current_pcb_executing->tid_count; i++)
            {
                t_tcb *tcb = &(current_pcb_executing->tcbs[i]); // Obtener el TCB

                // Enviar solicitud de eliminación de hilo a Memoria
                delete_thread_memory(tcb);

                if (current_pcb_executing->tid_count > 1)
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

            delete_process_memory(current_pcb_executing);
            seguir_ejecutando_mismo_tcb = false;

            // Liberar el array dinámico de TCBs
            free(current_pcb_executing->tcbs);
            current_pcb_executing->tcbs = NULL;
            current_pcb_executing->tid_count = 0;

            log_info(logger, "## Finaliza el proceso %i", current_pcb_executing->PID);
            move_pcb_to_exit(current_pcb_executing->PID);

            // mandar pcb a memoria
            // Funcion para avisar a memoria sobre la finalizacion del proceso <------------------
            destroy_process_exit(proceso_exit);

            try_create_NEW_process_loop();
            break;

        case THREAD_CREATE:

            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: THREAD_CREATE", current_pcb_executing->PID, proximo_tcb->TID);
            t_thread_create *hilo_create = deserialize_thread_create(buffer);
            // log_info(logger, "Nombre archivo pseudocodigo: %s", hilo_create->nombre_archivo);
            // log_info(logger, "La prioridad es: %d\n", hilo_create->prioridad);

            // PONER MUTEX PCB
            create_thread(current_pcb_executing, hilo_create->nombre_archivo, hilo_create->prioridad);

            // sem_post(&sem_thread_count);

            // Ver que hacer con el tcb que queda
            destroy_thread_create(hilo_create);

            break;

        case THREAD_JOIN:
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: THREAD_JOIN", current_pcb_executing->PID, proximo_tcb->TID);

            t_thread_operation *hilo_join = deserialize_thread_operation(buffer);

            // block_tcb_until_finish(current_tcb_executing, hilo_join->tid);
            t_tcb *blocking_tcb = find_tcb_from_tid(hilo_join->tid, proximo_tcb->parent_PID);

            log_info(logger, "Se busca el tcb tid: %d y ppid: %d", hilo_join->tid, proximo_tcb->parent_PID);

            if (blocking_tcb == NULL)
            {
                log_error(logger, "EL blocking_tcb es null, THREAD_JOIN abortado");
            }
            else
            {
                stop_quantum();
                log_warning(logger, "## (PID:TID) : (%i:%i) Esta bloqueando a: (PID:TID) : (%i:%i)", blocking_tcb->parent_PID, blocking_tcb->TID, proximo_tcb->parent_PID, proximo_tcb->TID);
                // log_error(logger, "El TCB bloqueante es: %d ", blocking_tcb->TID);

                add_tcb_join(proximo_tcb, blocking_tcb);

                // se bloquea
                seguir_ejecutando_mismo_tcb = false;
            }

            destroy_thread_operation(hilo_join);
            break;

        case THREAD_CANCEL:

            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: THREAD_CANCEL", current_pcb_executing->PID, proximo_tcb->TID);
            t_thread_operation *hilo_cancel = deserialize_thread_operation(buffer);
            // log_info(logger, "El tid es: %d\n", hilo_cancel->tid);
            // desbloquear_thread_por_join(hilo_cancel->tid);

            t_tcb *tcb = find_tcb_from_tid(hilo_cancel->tid, proximo_tcb->parent_PID);
            unlock_tcb_joins(tcb);

            destroy_thread_operation(hilo_cancel);
            break;

        case THREAD_EXIT:
            stop_quantum();
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: THREAD_EXIT", current_pcb_executing->PID, proximo_tcb->TID);
            t_thread_operation *hilo_exit = deserialize_thread_operation(buffer);
            // log_info(logger, "El tid es: %d El pid es: %d", hilo_exit->tid, current_pcb_executing->PID);

            // mandar el tcb a memoria para borrar el hilo
            delete_thread_memory(proximo_tcb);
            add_tcb_to_list(list_EXIT_TCB, proximo_tcb);

            // Actualizar contador de hilos del padre PCB
            t_pcb *parent_pcb = find_pcb_by_pid(proximo_tcb->parent_PID);
            if (parent_pcb != NULL)
            {
                int tid_count_original = parent_pcb->tid_count;
                parent_pcb->tid_count--;
                // log_debug(logger, "Se redujo el pid del padre de %d a %d", tid_count_original, parent_pcb->tid_count);
            }
            else
            {
                log_warning(logger, "No se encontró el PCB con PID = %d", proximo_tcb->parent_PID);
            }

            // desbloquear_thread_por_join(hilo_exit->tid);
            unlock_tcb_joins(proximo_tcb);

            // desbloquear todos los mutexes
            release_all_mutexes(proximo_tcb);
            seguir_ejecutando_mismo_tcb = false;

            destroy_thread_operation(hilo_exit);

            break;
        case MUTEX_CREATE:

            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: MUTEX_CREATE", current_pcb_executing->PID, proximo_tcb->TID);
            t_mutex_operation *mutex_syscall = deserialize_mutex_operation(buffer);
            // log_info(logger, "Nombre semaforp: %s", mutex_syscall->nombre_semaforo);
            // log_info(logger, "El tid es: %d\n", mutex_syscall->tid);

            mutex_create(mutex_syscall->nombre_semaforo, current_pcb_executing);

            destroy_mutex_operation(mutex_syscall);

            t_mutex *mutex_creado = list_get(global_mutexes, 0);
            // log_info(logger, "Nombre mutex creado: %s\n", mutex_creado->name);
            seguir_ejecutando_mismo_tcb = true;

            break;
        case MUTEX_LOCK:
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: MUTEX_LOCK", current_pcb_executing->PID, proximo_tcb->TID);
            t_mutex_operation *lock_mutex = deserialize_mutex_operation(buffer);
            // log_info(logger, "Nombre semaforp: %s ", lock_mutex->nombre_semaforo);
            // log_info(logger, "El tid es: %d\n", lock_mutex->tid);

            bool tcb_pudo_reservar_recurso = mutex_lock(lock_mutex->nombre_semaforo, proximo_tcb);

            if (!tcb_pudo_reservar_recurso)
            {
                stop_quantum();
            }

            destroy_mutex_operation(lock_mutex);
            seguir_ejecutando_mismo_tcb = tcb_pudo_reservar_recurso;
            break;
        case MUTEX_UNLOCK:
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: MUTEX_UNLOCK", current_pcb_executing->PID, proximo_tcb->TID);
            t_mutex_operation *unlock_mutex = deserialize_mutex_operation(buffer);
            // log_info(logger, "Nombre semaforp: %s", unlock_mutex->nombre_semaforo);
            // log_info(logger, "El tid es: %d\n", unlock_mutex->tid);

            mutex_unlock(unlock_mutex->nombre_semaforo, proximo_tcb);

            destroy_mutex_operation(unlock_mutex);
            seguir_ejecutando_mismo_tcb = true;
            break;

        case DUMP_MEMORY:
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: DUMP_MEMORY", current_pcb_executing->PID, proximo_tcb->TID);

            t_dump_memory *dump_memory = deserialize_dump_memory(buffer);

            stop_quantum();

            // Bloqueamos el Tcb en ejecucion

            pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
            log_info(logger, "## (PID:TID) : (%i:%i) - bloqueado por el dump memory", current_pcb_executing->PID, proximo_tcb->TID);
            add_tcb_to_list(list_BLOCKED_TCB, proximo_tcb);
            pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);

            seguir_ejecutando_mismo_tcb = false;

            pthread_t thread_id;

            // Crear una copia del TCB
            t_tcb *tcb_dump_mem = malloc(sizeof(t_tcb));
            if (tcb_dump_mem == NULL)
            {
                log_error(logger, "Error al alocar memoria para el TCB.");
                continue;
            }

            tcb_dump_mem->TID = proximo_tcb->TID;
            tcb_dump_mem->parent_PID = proximo_tcb->parent_PID;
            tcb_dump_mem->priority = proximo_tcb->priority;
            tcb_dump_mem->state = proximo_tcb->state;

            // Copiar la ruta del archivo
            tcb_dump_mem->file_path_length = proximo_tcb->file_path_length;
            tcb_dump_mem->file_path = malloc(tcb_dump_mem->file_path_length + 1); // +1 para el terminador nulo
            if (tcb_dump_mem->file_path != NULL)
            {
                strncpy(tcb_dump_mem->file_path, proximo_tcb->file_path, tcb_dump_mem->file_path_length);
                tcb_dump_mem->file_path[tcb_dump_mem->file_path_length] = '\0'; // Asegurar el terminador nulo
            }

            if (pthread_create(&thread_id, NULL, (void *)dump_memory_thread, (void *)tcb_dump_mem))
            {
                log_error(logger, "Error creando el hilo para el Memory Dump.");
                free(tcb_dump_mem); // Liberar memoria si no se pudo crear el hilo
                continue;
            }

            pthread_detach(thread_id);

            destroy_dump_memory(dump_memory);
            break;

        case IO:
            stop_quantum();
            log_info(logger, "## (PID:TID) : (%i:%i) - Solicitó syscall: IO", current_pcb_executing->PID, proximo_tcb->TID);
            t_syscall_io *syscall_io = deserialize_syscall_io(buffer);
            // log_info(logger, "Los milisegundos son: %d\n", syscall_io->milisegundos);
            request_io(current_tcb_executing, syscall_io->milisegundos);
            destroy_syscall_io(syscall_io);
            seguir_ejecutando_mismo_tcb = false;
            break;

        case SEGMENTATION_FAULT:
            stop_quantum();

            t_seg_fault *seg_fault = deserialize_seg_fault(buffer);

            log_error(logger, "Segmentation Fault (%d, %d)", seg_fault->pid, seg_fault->tid);

            for (uint32_t i = 0; i < current_pcb_executing->tid_count; i++)
            {
                t_tcb *tcb = &(current_pcb_executing->tcbs[i]); // Obtener el TCB

                // Enviar solicitud de eliminación de hilo a Memoria
                delete_thread_memory(tcb);

                if (current_pcb_executing->tid_count > 1)
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

            delete_process_memory(current_pcb_executing);
            seguir_ejecutando_mismo_tcb = false;

            // Liberar el array dinámico de TCBs
            free(current_pcb_executing->tcbs);
            current_pcb_executing->tcbs = NULL;
            current_pcb_executing->tid_count = 0;

            log_info(logger, "## Finaliza el proceso %i", current_pcb_executing->PID);
            move_pcb_to_exit(current_pcb_executing->PID);

            destroy_seg_fault(seg_fault);
            break;

        case KERNEL_INTERRUPTION:
            // log_info(logger, "Se recibio una syscall de kernel interruption");
            seguir_ejecutando_mismo_tcb = false;

            pthread_mutex_lock(&mutex_list_READY_TCB);    // Bloquear acceso a la lista READY.
            add_tcb_to_list(list_READY_TCB, proximo_tcb); // Añadir el PCB a la lista de READY.
            sem_post(&sem_thread_count);
            pthread_mutex_unlock(&mutex_list_READY_TCB); // Desbloquear la lista READY.

            log_info(logger, "## (PID:TID) : (%i:%i) - Desalojado por fin de Quantum", proximo_tcb->parent_PID, proximo_tcb->TID);
            stop_quantum();

            break;

        default:
            log_error(logger, "Unknown Operation Code");
            exit(1);
            break;
        }
        // TODO, ver esto bien
        free(buffer);
        previo_tcb = proximo_tcb;
    }
    free_connection(server_cpu_dispatch_fd);
}

void dispatch_thread(t_tcb *tcb, int client_fd)
{
    uint32_t tid = tcb->TID;
    uint32_t ppid = tcb->parent_PID;

    t_dispatch_request *dispatch_request = create_dispatch_request(tid, ppid);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_dispatch_request(dispatch_request, buffer);

    t_paquete *paquete_dispatch = create_package(DISPATCH);
    add_to_package(paquete_dispatch, buffer->stream, buffer->size);

    send_package(paquete_dispatch, client_fd);

    remove_tcb_from_list(list_READY_TCB, ppid, tid);
    current_tcb_executing = tcb;

    delete_package(paquete_dispatch);
    free(dispatch_request);
    destroy_buffer(buffer);
}