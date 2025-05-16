#ifndef UTILS_PCB_REQUEST_SERIALIZATION_H_
#define UTILS_PCB_REQUEST_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

typedef struct {
    uint32_t PID;               // Id entero autoincremental
    uint32_t size;              // Tamaño en memoria del proceso
} t_pcb_request;

t_pcb_request* create_pcb_request(uint32_t PID, uint32_t size);

void serialize_pcb_request(t_pcb_request* pcb_request, t_buffer* buffer);

t_pcb_request* deserialize_pcb_request(void* stream);

void destroy_pcb_request(t_pcb_request* pcb_request);
#endif