// pcb_serialization.c

#include "pcb_serialization.h"
#include <utils/estructuras.h>
#include <utils/communication.h>

extern t_log* logger;
uint32_t global_pid = 0;  // Variable global para PID autoincremental
t_pcb* create_pcb(uint32_t size) {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    
    if (!pcb) {
        log_error(logger, "Error al asignar memoria para t_pcb");
        exit(EXIT_FAILURE);
    }

    // Asignar PID global autoincremental
    pcb->PID = global_pid++;
    
    // El estado siempre será NEW
    pcb->state = NEW;
    
    // Asignar tamaño en memoria
    pcb->size = size;

    // Inicializar el array de mutexes vacío
    pcb->mutex = NULL;
    pcb->mutex_count = 0;

    // Inicializar el array de TIDs vacío
    pcb->TID = NULL;
    pcb->tid_count = 0;

    // Inicializar el array de TCBs vacío
    pcb->tcbs = NULL;
    pcb->num_tcbs = 0;

    return pcb;
}

void serialize_pcb(t_pcb* pcb, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular tamaño del buffer para TIDs, mutexes y TCBs
    size_t size = sizeof(uint32_t) + sizeof(t_state) +
                  (pcb->tid_count * sizeof(uint32_t)) +
                  (pcb->mutex_count * (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(bool))) + // Serializar mutex
                  sizeof(uint32_t) +  // num_tcbs
                  pcb->num_tcbs * sizeof(t_tcb) +
                  sizeof(uint32_t);  // size

    // Agregar el tamaño de las rutas de archivos
    for (uint32_t i = 0; i < pcb->num_tcbs; i++) {
        size += pcb->tcbs[i].file_path_length;
    }
    
    buffer->size = size;
    buffer->stream = malloc(size);

    if (!buffer->stream) {
        log_error(logger, "Error al asignar memoria para el buffer de serialización");
        exit(EXIT_FAILURE);
    }

    // Serializar PID
    memcpy(buffer->stream + buffer->offset, &(pcb->PID), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializar estado
    memcpy(buffer->stream + buffer->offset, &(pcb->state), sizeof(t_state));
    buffer->offset += sizeof(t_state);

    // Serializar el tamaño del proceso
    memcpy(buffer->stream + buffer->offset, &(pcb->size), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializar el array TID
    memcpy(buffer->stream + buffer->offset, pcb->TID, pcb->tid_count * sizeof(uint32_t));
    buffer->offset += pcb->tid_count * sizeof(uint32_t);

    // Serializar los mutexes
    for (uint32_t i = 0; i < pcb->mutex_count; i++) {
        t_mutex_pcb* mutex = &(pcb->mutex[i]);
        memcpy(buffer->stream + buffer->offset, &(mutex->name_length), sizeof(uint32_t));
        buffer->offset += sizeof(uint32_t);
        memcpy(buffer->stream + buffer->offset, mutex->name, mutex->name_length);
        buffer->offset += mutex->name_length;
        memcpy(buffer->stream + buffer->offset, &(mutex->assigned_tid), sizeof(uint32_t));
        buffer->offset += sizeof(uint32_t);
        memcpy(buffer->stream + buffer->offset, &(mutex->assigned), sizeof(bool));
        buffer->offset += sizeof(bool);
    }

    // Serializar el número de tcbs
    memcpy(buffer->stream + buffer->offset, &(pcb->num_tcbs), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializar los TCBs
    for (uint32_t i = 0; i < pcb->num_tcbs; i++) {
        t_tcb* tcb = &(pcb->tcbs[i]);
        memcpy(buffer->stream + buffer->offset, &(tcb->TID), sizeof(uint32_t));
        buffer->offset += sizeof(uint32_t);
        memcpy(buffer->stream + buffer->offset, &(tcb->priority), sizeof(uint32_t));
        buffer->offset += sizeof(uint32_t);
        memcpy(buffer->stream + buffer->offset, &(tcb->state), sizeof(t_state));
        buffer->offset += sizeof(t_state);
        memcpy(buffer->stream + buffer->offset, &(tcb->file_path_length), sizeof(uint32_t));
        buffer->offset += sizeof(uint32_t);
        memcpy(buffer->stream + buffer->offset, tcb->file_path, tcb->file_path_length);
        buffer->offset += tcb->file_path_length;
    }
}

t_pcb* deserialize_pcb(void* stream) {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    int offset = 0;

    // Deserializar PID
    memcpy(&(pcb->PID), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializar estado
    memcpy(&(pcb->state), stream + offset, sizeof(t_state));
    offset += sizeof(t_state);

    // Deserializar tamaño del proceso
    memcpy(&(pcb->size), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializar el array TID
    uint32_t tid_size;
    memcpy(&tid_size, stream + offset, sizeof(uint32_t));
    pcb->TID = malloc(tid_size * sizeof(uint32_t));
    memcpy(pcb->TID, stream + offset, tid_size * sizeof(uint32_t));
    pcb->tid_count = tid_size;
    offset += tid_size * sizeof(uint32_t);

    // Deserializar los mutexes
    uint32_t mutex_size;
    memcpy(&mutex_size, stream + offset, sizeof(uint32_t));
    pcb->mutex = malloc(mutex_size * sizeof(t_mutex_pcb));
    pcb->mutex_count = mutex_size;
    for (uint32_t i = 0; i < mutex_size; i++) {
        t_mutex_pcb* mutex = &(pcb->mutex[i]);
        memcpy(&(mutex->name_length), stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        mutex->name = malloc(mutex->name_length);
        memcpy(mutex->name, stream + offset, mutex->name_length);
        offset += mutex->name_length;
        memcpy(&(mutex->assigned_tid), stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&(mutex->assigned), stream + offset, sizeof(bool));
        offset += sizeof(bool);
    }

    // Deserializar el número de tcbs
    memcpy(&(pcb->num_tcbs), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializar los TCBs
    pcb->tcbs = malloc(pcb->num_tcbs * sizeof(t_tcb));
    for (uint32_t i = 0; i < pcb->num_tcbs; i++) {
        t_tcb* tcb = &(pcb->tcbs[i]);
        memcpy(&(tcb->TID), stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&(tcb->priority), stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&(tcb->state), stream + offset, sizeof(t_state));
        offset += sizeof(t_state);
        memcpy(&(tcb->file_path_length), stream + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        tcb->file_path = malloc(tcb->file_path_length);
        memcpy(tcb->file_path, stream + offset, tcb->file_path_length);
        offset += tcb->file_path_length;
    }

    return pcb;
}

void delete_pcb(t_pcb* pcb) {
    // Liberar memoria del array TID si está asignado
    if (pcb->TID) {
        free(pcb->TID);
    }

    // Liberar memoria del array de mutexes si está asignado
    if (pcb->mutex) {
        for (uint32_t i = 0; i < pcb->mutex_count; i++) {
            free(pcb->mutex[i]->name); // Liberar el nombre del mutex
        }
        free(pcb->mutex); // Liberar el array de mutexes
    }

    // Liberar memoria del array de tcbs si está asignado
    if (pcb->tcbs) {
        for (uint32_t i = 0; i < pcb->num_tcbs; i++) {
            free(pcb->tcbs[i].file_path); // Liberar la memoria de la ruta de archivo de cada TCB
        }
        free(pcb->tcbs);
    } 

    // Liberar memoria del pcb
    free(pcb);
}