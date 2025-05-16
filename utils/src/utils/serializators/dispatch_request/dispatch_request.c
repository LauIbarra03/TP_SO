#include "dispatch_request.h"

t_dispatch_request* create_dispatch_request(u_int32_t tid, u_int32_t pid) {
    t_dispatch_request *request = malloc(sizeof(t_dispatch_request));

    if (request == NULL) {
        return NULL;
    }

    request->tid = tid;
    request->pid = pid;

    return request;
}

// Función para serializar un t_dispatch_request
void serialize_dispatch_request(t_dispatch_request* request, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño de la serialización (2 u_int32_t)
    size_t size = sizeof(u_int32_t) * 2;
    buffer->size = size;

    // Reservar memoria para el stream
    buffer->stream = malloc(size);

    // Serializamos el tid
    memcpy(buffer->stream + buffer->offset, &(request->tid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el pid
    memcpy(buffer->stream + buffer->offset, &(request->pid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);
}

// Función para deserializar un t_dispatch_request
t_dispatch_request* deserialize_dispatch_request(void* stream) {
    t_dispatch_request* request = malloc(sizeof(t_dispatch_request));
    int offset = 0;

    // Deserializamos el tid
    memcpy(&(request->tid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el pid
    memcpy(&(request->pid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    return request;
}

// Función para destruir un t_dispatch_request
void destroy_dispatch_request(t_dispatch_request* request) {
    if (request != NULL) {
        free(request);
    }
}