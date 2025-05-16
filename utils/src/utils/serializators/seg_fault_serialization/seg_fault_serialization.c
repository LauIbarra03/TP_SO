#include "seg_fault_serialization.h"

t_seg_fault* create_seg_fault(u_int32_t tid, u_int32_t pid, op_code code) {
    t_seg_fault *seg_fault = malloc(sizeof(t_seg_fault));

    if (seg_fault == NULL) {
        return NULL;
    }

    seg_fault->tid = tid;
    seg_fault->pid = pid;
    seg_fault->code = code;

    return seg_fault;
}

void serialize_seg_fault(t_seg_fault* seg_fault, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño de la serialización (2 u_int32_t y 1 op_code)
    size_t size = sizeof(u_int32_t) * 2 + sizeof(op_code);
    buffer->size = size;

    // Reservar memoria para el stream
    buffer->stream = malloc(size);

    // Serializamos el tid
    memcpy(buffer->stream + buffer->offset, &(seg_fault->tid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el pid
    memcpy(buffer->stream + buffer->offset, &(seg_fault->pid), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el código de operación (op_code)
    memcpy(buffer->stream + buffer->offset, &(seg_fault->code), sizeof(op_code));
    buffer->offset += sizeof(op_code);
}

t_seg_fault* deserialize_seg_fault(void* stream) {
    t_seg_fault* seg_fault = malloc(sizeof(t_seg_fault));
    int offset = 0;

    // Deserializamos el tid
    memcpy(&(seg_fault->tid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el pid
    memcpy(&(seg_fault->pid), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Deserializamos el código de operación (op_code)
    memcpy(&(seg_fault->code), stream + offset, sizeof(op_code));
    offset += sizeof(op_code);

    return seg_fault;
}

void destroy_seg_fault(t_seg_fault* seg_fault) {
    if (seg_fault != NULL) {
        free(seg_fault);
    }
}