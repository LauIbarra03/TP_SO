#include "write_mem_request_serialization.h"

t_write_mem_request* create_write_mem_request(uint32_t tid, uint32_t pid, uint32_t physical_address, uint32_t data_register_value) {
    t_write_mem_request* write_mem_request = malloc(sizeof(t_write_mem_request));

    if (write_mem_request == NULL) {
        return NULL;
    }

    write_mem_request->tid = tid;
    write_mem_request->pid = pid;
    write_mem_request->physical_address = physical_address;
    write_mem_request->data_register_value = data_register_value;

    return write_mem_request;
}

void serialize_write_mem_request(t_write_mem_request* write_mem_request, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño de la serialización (4 uint32_t)
    size_t size = sizeof(uint32_t) * 4;
    buffer->size = size;

    // Reservar memoria para el stream
    buffer->stream = malloc(size);

    // Serializamos el physical_address
    memcpy(buffer->stream + buffer->offset, &(write_mem_request->physical_address), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos el pid
    memcpy(buffer->stream + buffer->offset, &(write_mem_request->pid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos el tid
    memcpy(buffer->stream + buffer->offset, &(write_mem_request->tid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    // Serializamos el data_register_value
    memcpy(buffer->stream + buffer->offset, &(write_mem_request->data_register_value), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);
}

t_write_mem_request* deserialize_write_mem_request(void* stream) {
    t_write_mem_request* write_mem_request = malloc(sizeof(t_write_mem_request));
    int offset = 0;

    // Deserializamos el physical_address
    memcpy(&(write_mem_request->physical_address), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos el pid
    memcpy(&(write_mem_request->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos el tid
    memcpy(&(write_mem_request->tid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Deserializamos el data_register_value
    memcpy(&(write_mem_request->data_register_value), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return write_mem_request;
}

void destroy_write_mem_request(t_write_mem_request* write_mem_request) {
    if (write_mem_request != NULL) {
        free(write_mem_request);
    }
}