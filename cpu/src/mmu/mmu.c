#include "mmu.h"

int translate_address(t_execution_context* execution_context, u_int32_t logical_address, int memory_fd, int kernel_fd) {
    u_int32_t base   = execution_context->registers->BASE;
    u_int32_t limite = execution_context->registers->LIMITE;
    u_int32_t physical_address;

    if (logical_address >= limite) {
        log_error(logger, "No es posible hacer la traduccion: SEGMENTATION FAULT (dir lógica: %u, límite: %u)", logical_address, limite);
        update_execution_context(execution_context, memory_fd);
        /* t_seg_fault *seg_fault = create_seg_fault(execution_context->tid, execution_context->pid, SEGMENTATION_FAULT);
        send_segmentation_fault(seg_fault, kernel_fd);
        destroy_seg_fault(seg_fault); */
        return SEG_FAULT;
    }

    physical_address = base + logical_address;    
    return physical_address;
}

void send_segmentation_fault(t_seg_fault *seg_fault, int kernel_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->operation_code = SEGMENTATION_FAULT;
    paquete->buffer = malloc(sizeof(t_buffer));

    serialize_seg_fault(seg_fault, paquete->buffer);

    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);

    send(kernel_fd, a_enviar, bytes, 0);

    free(a_enviar);
    delete_package(paquete);
}
