#ifndef UTILS_SEG_FAULT_SERIALIZATION_H_
#define UTILS_SEG_FAULT_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    u_int32_t tid;
    u_int32_t pid;
    op_code code;
} t_seg_fault;


void serialize_seg_fault(t_seg_fault* seg_fault, t_buffer* buffer);

t_seg_fault*  deserialize_seg_fault(void* stream);

t_seg_fault*  create_seg_fault(u_int32_t tid, u_int32_t pid, op_code code);

void destroy_seg_fault(t_seg_fault* seg_fault);


#endif

