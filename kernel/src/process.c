// process.c

#include "process.h"
#include <utils/estructuras.h>
#include <utils/pcb_serialization.h>
#include <utils/tcb_serialization.h>
#include <utils/pcb_misc.h>
#include <utils/communication.h>

#include <semaphore.h>

#include <pcb_lists.h>
#include <tcb_lists.h>
#include <scheduler.h>

const char *file_path = "example.txt";

extern t_log *logger;
extern t_config *config;

// Declaración de colas para TCBs
extern t_list *list_NEW_TCB;
extern t_list *list_READY_TCB;
extern t_list *list_BLOCKED_TCB;
extern t_list *list_EXIT_TCB;

extern sem_t sem_thread_count;

extern pthread_mutex_t mutex_list_NEW_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB;

void init_proceso_inicial(char *path, uint32_t size)
{
    op_code res_mem = create_process(size, path, 0);

    try_create_NEW_process_loop();
    
    if (res_mem == OK)
    {
        // PONER MUTEX PCB
        current_pcb_executing = list_remove(list_READY_TCB, 0);
        log_info(logger, "Proceso inicial creado con exito");
    }
    else
    {
        log_info(logger, "Error en crear el proceso incial");
    }
}

op_code create_process_memory(t_pcb *pcb)
{

    t_pcb_request* pcb_request = create_pcb_request(pcb->PID, pcb->size);

    // 1. Crear buffer y serializar el PCB request
    t_buffer *buffer = malloc(sizeof(t_buffer));
    // serialize_pcb(pcb, buffer);
    serialize_pcb_request(pcb_request, buffer); // Serializar el PCB request en el buffer

    // 2. Crear el paquete con un op_code específico para el PCB request
    t_paquete *paquete = create_package(PROCESS_CREATE); // Define el OP_CODE para enviar un PCB request

    // 3. Agregar el buffer serializado al paquete
    add_to_package(paquete, buffer->stream, buffer->size);

    // 4. Establecer la conexión con memoria (string vacío por ahora)
    int memory_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");

    if (memory_connection == -1)
    {
        log_error(logger, "Error al conectar con Memoria");
        free(buffer->stream);
        free(buffer);
        delete_package(paquete);
        return ERROR;
    }

    // 5. Enviar el paquete a memoria
    send_package(paquete, memory_connection);

    // 6. Esperar la respuesta de memoria, y una vez respondida, desconectar
    int response = wait_for_response(memory_connection);
    send_response(memory_connection, DISCONNECT);

    // 7. Liberar memoria
    delete_package(paquete);            // Liberar el paquete
    free(buffer->stream);               // Liberar el stream
    free(buffer);                       // Liberar el buffer
    free_connection(memory_connection); // Liberar la conexión
    destroy_pcb_request(pcb_request);

    if (response == OK)
    {
        log_info(logger, "Creación del proceso en memoria exitosa");
    }
    else
    {
        log_error(logger, "Error en la creación del proceso");
        return ERROR;
    }

    return OK;
}

op_code create_thread_memory(t_tcb *tcb)
{

    // 1. Crear buffer y serializar el TCB
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_tcb(tcb, buffer);

    // 2. Crear el paquete con un op_code específico para el tcb
    t_paquete *paquete = create_package(THREAD_CREATE); // Define el OP_CODE para enviar un tcb

    // 3. Agregar el buffer serializado al paquete
    add_to_package(paquete, buffer->stream, buffer->size);

    // 4. Establecer la conexión con memoria (string vacío por ahora)
    int memory_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");

    if (memory_connection == -1)
    {
        log_error(logger, "Error al conectar con Memoria");
        free(buffer->stream);
        free(buffer);
        delete_package(paquete);
        return ERROR;
    }

    // 5. Enviar el paquete a memoria
    send_package(paquete, memory_connection);

    // 6. Esperar la respuesta de memoria, y una vez respondida, desconectar
    int response = wait_for_response(memory_connection);
    send_response(memory_connection, DISCONNECT);

    // 7. Liberar memoria
    delete_package(paquete);            // Liberar el paquete
    free(buffer->stream);               // Liberar el stream
    free(buffer);                       // Liberar el buffer
    free_connection(memory_connection); // Liberar la conexión

    if (response == OK)
    {
        log_info(logger, "Creación del hilo en memoria exitosa");
    }
    else
    {
        log_error(logger, "Error en la creación del hilo");
        return ERROR;
    }

    return OK;
}

op_code create_process(uint32_t size, char *path, int priority)
{
    // Crear el PCB y asociar los TCBs
    t_pcb *pcb = create_pcb(size);
    

    add_pcb_to_new(pcb);
}

t_tcb *create_thread(t_pcb *pcb, char *path, int priority)
{
    uint32_t new_tid = pcb->tid_count;

    t_tcb *tcb = create_tcb(new_tid, priority, path);

    add_tcb_to_pcb(pcb, tcb);

    add_tcb_to_list(list_NEW_TCB, tcb);

    log_info(logger, "## (PID:TID) : (%i:%i) Se crea el Hilo - Estado: READY", tcb->parent_PID, tcb->TID);

    int respuesta_memoria = create_thread_memory(tcb);

    if (respuesta_memoria == OK)
    {
        // Muevo el tcb de NEW a READY
        pthread_mutex_lock(&mutex_list_NEW_TCB);
        pthread_mutex_lock(&mutex_list_READY_TCB);

        // log_debug(logger, "Añadiendo elemento a la lista READY");
        
        move_tcb_between_lists(list_NEW_TCB, list_READY_TCB, pcb->PID, tcb->TID, READY);

        sem_post(&sem_thread_count);

        pthread_mutex_unlock(&mutex_list_READY_TCB);
        pthread_mutex_unlock(&mutex_list_NEW_TCB);
    }
    else
    {
        // En caso de error, muevo el tcb de NEW a EXIT
        log_error(logger, "No se pudo crear el hilo en memoria. Dejando el tcb en NEW.");
    }

    return tcb;
}

void log_pcb_threads_info(t_pcb *pcb)
{
    log_info(logger, "\n");
    if (pcb == NULL)
    {
        log_error(logger, "El PCB proporcionado es NULL");
        return;
    }

    log_info(logger, "Información de los hilos para el proceso con PID: %d", pcb->PID);
    for (uint32_t i = 0; i < pcb->num_tcbs; i++)
    {
        t_tcb *tcb = &(pcb->tcbs[i]);

        log_info(logger, "Hilo %d:", tcb->TID);
        log_info(logger, "  Prioridad: %d", tcb->priority);
        log_info(logger, "  TID: %d", tcb->TID);
        log_info(logger, "  Ruta de archivo: %s", tcb->file_path);
        log_info(logger, "  Longitud de la ruta: %d", tcb->file_path_length);
    }
    log_info(logger, "\n");
}

void log_pcb_tids(t_pcb *pcb)
{
    log_info(logger, "\n");
    if (pcb == NULL)
    {
        log_error(logger, "El PCB proporcionado es NULL");
        return;
    }

    if (pcb->tid_count == 0 || pcb->TID == NULL)
    {
        log_info(logger, "El proceso con PID: %d no tiene TIDs asociados.", pcb->PID);
        return;
    }

    log_info(logger, "TIDs para el proceso con PID: %d:", pcb->PID);
    for (uint32_t i = 0; i < pcb->tid_count; i++)
    {
        log_info(logger, "  TID[%d]: %d", i, pcb->TID[i]);
    }
    log_info(logger, "\n");
}

/*t_mutex_pcb* create_mutex(const char* name) {

    t_mutex_pcb* mutex = malloc(sizeof(t_mutex_pcb));

    if (!mutex) {
        log_error(logger, "Error al asignar memoria para t_mutex_pcb");
        exit(EXIT_FAILURE);
    }

    // Asignar el nombre del mutex
    mutex->name_length = strlen(name) + 1;
    mutex->name = malloc(mutex->name_length);
    strcpy(mutex->name, name);

    // Inicializar el TID y estado del mutex
    mutex->assigned_tid = -1;
    mutex->assigned = false;

    return mutex;
}*/

/*void add_mutex_to_pcb(t_pcb* pcb, t_mutex_pcb* mutex) {
    // Aumentar el tamaño del array de mutexes
    pcb->mutex_count++;
    pcb->mutex = realloc(pcb->mutex, pcb->mutex_count * sizeof(t_mutex_pcb));

    if (!pcb->mutex) {
        log_error(logger, "Error al reasignar memoria para el array de mutexes en el PCB");
        exit(EXIT_FAILURE);
    }

    // Añadir el nuevo mutex al array
    pcb->mutex[pcb->mutex_count - 1] = *mutex;
} */

void log_mutex_pcb(t_pcb *pcb)
{
    log_info(logger, "\n");
    if (pcb == NULL)
    {
        log_error(logger, "El PCB proporcionado es NULL");
        return;
    }

    if (pcb->mutex_count == 0 || pcb->mutex == NULL)
    {
        log_info(logger, "El proceso con PID: %d no tiene mutexes asociados.", pcb->PID);
        return;
    }

    log_info(logger, "Mutexes para el proceso con PID: %d", pcb->PID);
    for (uint32_t i = 0; i < pcb->mutex_count; i++)
    {
        t_mutex_pcb *mutex = &(pcb->mutex[i]);
        log_info(logger, "Mutex %d:", i);
        log_info(logger, "  Nombre: %s", mutex->name);
        log_info(logger, "  Largo del nombre: %d", mutex->name_length);
        log_info(logger, "  TID asignado: %d", mutex->assigned_tid);
        log_info(logger, "  Asignado: %s", mutex->assigned ? "true" : "false");
    }
    log_info(logger, "\n");
}

op_code delete_process_memory(t_pcb *pcb)
{
    if (pcb == NULL)
    {
        log_error(logger, "El PCB proporcionado es NULL");
        return ERROR;
    }

    t_pcb_request* pcb_request = create_pcb_request(pcb->PID, pcb->size);

    // 1. Crear buffer y serializar el PCB request
    t_buffer *buffer = malloc(sizeof(t_buffer));
    // serialize_pcb(pcb, buffer);
    serialize_pcb_request(pcb_request, buffer); // Serializar el PCB request en el buffer

    // 2. Crear el paquete con el opcode específico para eliminar un proceso
    t_paquete *paquete = create_package(PROCESS_EXIT); // Define el OP_CODE para eliminar un PCB request

    // 3. Agregar el buffer serializado al paquete
    add_to_package(paquete, buffer->stream, buffer->size);

    // 4. Establecer la conexión con memoria
    int memory_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");

    if (memory_connection == -1)
    {
        log_error(logger, "Error al conectar con Memoria");
        free(buffer->stream);
        free(buffer);
        delete_package(paquete);
        return ERROR;
    }

    // 5. Enviar el paquete a memoria
    send_package(paquete, memory_connection);

    // 6. Esperar la respuesta de memoria, y una vez respondida, desconectar
    int response = wait_for_response(memory_connection);
    send_response(memory_connection, DISCONNECT);

    // 7. Liberar memoria
    delete_package(paquete);
    free(buffer->stream);
    free(buffer);
    free_connection(memory_connection);
    destroy_pcb_request(pcb_request);

    if (response == OK)
    {
        log_info(logger, "Eliminación del proceso en memoria exitosa (PID = %d)", pcb->PID);
        // delete_pcb(pcb);
    }
    else
    {
        log_error(logger, "Error en la eliminación del proceso (PID = %d)", pcb->PID);
        return ERROR;
    }

    return OK;
}

op_code delete_thread_memory(t_tcb *tcb)
{
    if (tcb == NULL)
    {
        log_error(logger, "El TCB proporcionado es NULL");
        return ERROR;
    }

    // 1. Crear buffer y serializar el TCB
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_tcb(tcb, buffer); // Serializar el TCB en el buffer

    // 2. Crear el paquete con el opcode específico para eliminar un hilo
    t_paquete *paquete = create_package(THREAD_EXIT); // Define el OP_CODE para eliminar un TCB

    // 3. Agregar el buffer serializado al paquete
    add_to_package(paquete, buffer->stream, buffer->size);

    // 4. Establecer la conexión con memoria
    int memory_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");

    if (memory_connection == -1)
    {
        log_error(logger, "Error al conectar con Memoria");
        free(buffer->stream);
        free(buffer);
        delete_package(paquete);
        return ERROR;
    }

    // 5. Enviar el paquete a memoria
    send_package(paquete, memory_connection);

    // 6. Esperar la respuesta de memoria, y una vez respondida, desconectar
    int response = wait_for_response(memory_connection);
    send_response(memory_connection, DISCONNECT);

    // 7. Liberar memoria
    delete_package(paquete);
    free(buffer->stream);
    free(buffer);
    free_connection(memory_connection);

    if (response == OK)
    {
        log_info(logger, "## (PID:TID) : (%i:%i) Finaliza el hilo", tcb->parent_PID, tcb->TID);
        // delete_tcb(tcb);
    }
    else
    {
        log_error(logger, "Error en la eliminación del hilo (TID = %d, PID = %d)", tcb->TID, tcb->parent_PID);
        return ERROR;
    }

    return OK;
}
