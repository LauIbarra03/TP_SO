//tcb_serialization.c

#include "tcb_serialization.h"
#include <utils/estructuras.h>
#include <utils/communication.h>
#include <stdint.h>
#include <string.h>
#include <commons/collections/queue.h>  // Cambiado de list.h a queue.h
#include <semaphore.h>
#include <pthread.h>
#include <../../kernel/src/process.h>

//#include "../../kernel/src/tcb_lists.h"
#include "../../kernel/src/scheduler.h" // Incluir para usar el semáforo

// Aseguramos que logger esté disponible externamente
extern t_log* logger;

extern pthread_mutex_t mutex_list_NEW_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;
extern pthread_mutex_t mutex_list_BLOCKED_TCB;
extern pthread_mutex_t mutex_list_EXIT_TCB;

// Declaración de colas para TCBs
/*extern t_list *list_NEW_TCB;
extern t_list *list_READY_TCB;
extern t_list *list_BLOCKED_TCB;
extern t_list *list_EXIT_TCB; */

// Serializa un t_tcb en un buffer
void serialize_tcb(t_tcb* tcb, t_buffer* buffer) {
    buffer->offset = 0;

    // Calculamos el tamaño total del buffer: TID + parent_PID + priority + state + file_path_length + file_path
    size_t size = sizeof(uint32_t) * 4 + sizeof(t_state) + tcb->file_path_length;
    buffer->size = size;

    // Asignamos memoria para el stream del buffer
    buffer->stream = malloc(size);

    if (!buffer->stream) {
        log_error(logger, "Error al asignar memoria para el buffer");
        exit(EXIT_FAILURE);
    }

    // Serializamos el TID
    memcpy(buffer->stream + buffer->offset, &(tcb->TID), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos el parent_PID
    memcpy(buffer->stream + buffer->offset, &(tcb->parent_PID), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos la prioridad
    memcpy(buffer->stream + buffer->offset, &(tcb->priority), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos el estado
    memcpy(buffer->stream + buffer->offset, &(tcb->state), sizeof(t_state));
    buffer->offset += sizeof(t_state);

    // Serializamos la longitud de la ruta de archivo
    memcpy(buffer->stream + buffer->offset, &(tcb->file_path_length), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos la ruta de archivo
    memcpy(buffer->stream + buffer->offset, tcb->file_path, tcb->file_path_length);
    buffer->offset += tcb->file_path_length;
}

// Deserializa un buffer en un t_tcb
t_tcb* deserialize_tcb(void* stream) {
    t_tcb* tcb = malloc(sizeof(t_tcb));
    
    if (!tcb) {
        log_error(logger, "Error al asignar memoria para t_tcb");
        delete_tcb(tcb);
        exit(EXIT_FAILURE);
    }

    int offset = 0;

    // Deserializamos el TID
    memcpy(&(tcb->TID), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos el parent_PID
    memcpy(&(tcb->parent_PID), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos la prioridad
    memcpy(&(tcb->priority), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos el estado
    memcpy(&(tcb->state), stream + offset, sizeof(t_state));
    offset += sizeof(t_state);

    // Deserializamos la longitud de la ruta de archivo
    memcpy(&(tcb->file_path_length), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Verificar la longitud antes de asignar memoria
    if (tcb->file_path_length > 0) {
        // Asignamos memoria para la ruta de archivo y deserializamos
        tcb->file_path = malloc(tcb->file_path_length);
        if (!tcb->file_path) {
            log_error(logger, "Error al asignar memoria para la ruta de archivo");
            destroy_tcb(tcb);
            exit(EXIT_FAILURE);
        }
        memcpy(tcb->file_path, stream + offset, tcb->file_path_length);
        offset += tcb->file_path_length;
    } else {
        tcb->file_path = NULL;
    }

    return tcb;
}

// Crea un nuevo t_tcb y envía un signal al semáforo del planificador a largo plazo
t_tcb* create_tcb(uint32_t TID, uint32_t priority, const char *file_path) {
    t_tcb* tcb = malloc(sizeof(t_tcb));

    if (!tcb) {
        log_error(logger, "Error al asignar memoria para t_tcb");
        exit(EXIT_FAILURE);
    }

    // Asignar el TID proporcionado (no global)
    tcb->TID = TID;

    // Inicializar el PID del proceso padre con -1
    tcb->parent_PID = (uint32_t) -1;
    
    // El estado siempre será NEW
    tcb->state = NEW;

    // Asignar la prioridad
    tcb->priority = priority;

    // Asignar la ruta de archivo y su longitud
    tcb->file_path_length = strlen(file_path) + 1; // +1 para incluir el carácter nulo
    tcb->file_path = malloc(tcb->file_path_length);
    if (!tcb->file_path) {
        log_error(logger, "Error al asignar memoria para la ruta de archivo");
        free(tcb);
        exit(EXIT_FAILURE);
    }
    memcpy(tcb->file_path, file_path, tcb->file_path_length);

    

    return tcb;
}

void delete_tcb(t_tcb* tcb) {
    if (tcb == NULL) {
        return; // No hacer nada si el puntero es NULL
    }
    
    // Liberar el campo dinámico file_path, si no es NULL
    if (tcb->file_path != NULL) {
        free(tcb->file_path);
        tcb->file_path = NULL; // Prevenir accesos posteriores inválidos
    }
    
    // Liberar el propio tcb
    free(tcb);
}


// Elimina un t_tcb y lo quita de la lista en la que esté
void destroy_tcb(t_tcb* tcb) {
    /* if (tcb) {
        // Remover el TCB de la lista correspondiente antes de destruirlo
        t_queue *target_queue = NULL;

        switch (tcb->state) {
            case NEW:
                target_queue = list_NEW_TCB;
                break;
            case READY:
                target_queue = list_READY_TCB;
                break;
            case BLOCKED:
                target_queue = list_BLOCKED_TCB;
                break;
            case EXIT:
                target_queue = list_EXIT_TCB;
                break;
            default:
                log_warning(logger, "Estado de TCB desconocido.");
                break;
        }

        if (target_queue != NULL) {
            t_tcb *removed_tcb = find_tcb_in_list(target_queue->elements, tcb->parent_PID, tcb->TID);
            if (removed_tcb != NULL) {
                queue_pop(target_queue);  // No hay una función directa para "remover por condición", por lo que hacemos pop.
                log_info(logger, "TCB con PID %d y TID %d removido de la lista", tcb->parent_PID, tcb->TID);
            }
        }

        if (tcb->file_path) {
            free(tcb->file_path); // Liberar la memoria de la ruta de archivo
        }
        free(tcb); // Liberar la memoria del tcb
    } */
}
