#ifndef MMU_H
#define MMU_H

// Utils
#include "utils/communication.h"
#include "utils/estructuras.h"
#include "utils/server.h"
#include "utils/message_serialization.h"
#include "utils/serializators/execution_context_serialization/execution_context_serialization.h"
#include "utils/serializators/seg_fault_serialization/seg_fault_serialization.h"


// Commons 
#include <commons/log.h>

// Del propio modulo
#include <execution_context/execution_context.h>
#include <defines.h>
// Variables

// Funciones principales
int translate_address(t_execution_context* execution_context, u_int32_t logical_address, int memory_fd, int kernel_fd);

// Funciones auxiliares
void send_segmentation_fault(t_seg_fault *seg_fault, int kernel_fd);

#endif