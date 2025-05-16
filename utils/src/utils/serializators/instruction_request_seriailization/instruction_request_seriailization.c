#include "instruction_request_seriailization.h"

t_instruction_request* create_instruction_request(u_int32_t tid, u_int32_t pid, u_int32_t pc) {
    t_instruction_request *req = malloc(sizeof(t_instruction_request));

    if (req == NULL) {
        return NULL;
    }
    
    req->tid = tid;
    req->pc = pc;
    req->pid = pid;
    return req;
}


void serialize_instruction_request(t_instruction_request* request, t_buffer* buffer) {
    buffer->offset = 0;

    size_t size = sizeof(u_int32_t) * 3; 
    buffer->size = size;

    buffer->stream = malloc(size);

    // Serializamos el tid
    memcpy(buffer->stream + buffer->offset, &(request->tid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el pc
    memcpy(buffer->stream + buffer->offset, &(request->pc), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el pid
    memcpy(buffer->stream + buffer->offset, &(request->pid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);
}

t_instruction_request* deserialize_instruction_request(void* stream) {
    t_instruction_request* request = malloc(sizeof(t_instruction_request));
    int offset = 0;

    // Deserializamos el tid
    memcpy(&(request->tid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el pc
    memcpy(&(request->pc), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el pid
    memcpy(&(request->pid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    return request;
}

void destroy_instruction_request(t_instruction_request* request) {
    if (request != NULL) {
        free(request);
    }
    
}