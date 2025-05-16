#include "pcb_request_serialization.h"

t_pcb_request* create_pcb_request(uint32_t PID, uint32_t size) {
    t_pcb_request* new_pcb_request = malloc(sizeof(t_pcb_request));
    new_pcb_request->PID = PID;
    new_pcb_request->size = size;

    return new_pcb_request;
}

void serialize_pcb_request(t_pcb_request* pcb_request, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño del stream
    size_t total_size = sizeof(uint32_t) + sizeof(uint32_t); // PID + size
    buffer->size = total_size;

    buffer->stream = malloc(total_size);

    // Copiar cada campo al stream
    memcpy(buffer->stream + buffer->offset, &(pcb_request->PID), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, &(pcb_request->size), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_pcb_request* deserialize_pcb_request(void* stream) {
    t_pcb_request* pcb_request = malloc(sizeof(t_pcb_request));
    int offset = 0;

    // Leer el PID
    memcpy(&(pcb_request->PID), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Leer el tamaño
    memcpy(&(pcb_request->size), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return pcb_request;
}

void destroy_pcb_request(t_pcb_request* pcb_request) {
    if (pcb_request != NULL) {
        free(pcb_request); // Liberar la estructura
    }
}