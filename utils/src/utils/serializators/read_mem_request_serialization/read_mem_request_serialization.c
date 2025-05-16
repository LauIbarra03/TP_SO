#include "read_mem_request_serialization.h"

t_read_mem_request* create_read_mem_request(u_int32_t tid, u_int32_t pid, uint32_t physical_address) {
    t_read_mem_request* read_mem_request = malloc(sizeof(t_read_mem_request));

    if (read_mem_request == NULL) {
        return NULL;
    }

    read_mem_request->tid = tid;
    read_mem_request->pid = pid;
    read_mem_request->physical_address = physical_address;

    return read_mem_request;
}

void serialize_read_mem_request(t_read_mem_request* read_mem_request, t_buffer* buffer){
    buffer->offset = 0;

    // Calcular el tamaño de la serialización (3 u_int32_t)
    size_t size = sizeof(u_int32_t) * 3;
    buffer->size = size;

    // Reservar memoria para el stream
    buffer->stream = malloc(size);

    // Serializamos el physical_address
    memcpy(buffer->stream + buffer->offset, &(read_mem_request->physical_address), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el pid
    memcpy(buffer->stream + buffer->offset, &(read_mem_request->pid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el tid
    memcpy(buffer->stream + buffer->offset, &(read_mem_request->tid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);
}

t_read_mem_request* deserialize_read_mem_request(void* stream){
    t_read_mem_request* read_mem_request = malloc(sizeof(t_read_mem_request));
    int offset = 0;

    // Deserializamos el physical_address
    memcpy(&(read_mem_request->physical_address), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el pid
    memcpy(&(read_mem_request->pid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el tid
    memcpy(&(read_mem_request->tid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    return read_mem_request;
}

void destroy_read_mem_request(t_read_mem_request* read_mem_request) {
    if (read_mem_request != NULL) {
        free(read_mem_request);
    }
}