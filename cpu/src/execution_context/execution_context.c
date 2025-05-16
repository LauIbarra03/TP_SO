#include "execution_context.h"

void request_execution_context(u_int32_t tid, u_int32_t pid , int memory_fd) {

    t_dispatch_request *req = create_dispatch_request(tid, pid);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_dispatch_request(req, buffer);

    t_paquete *package = create_package(EXECUTION_CONTEXT);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "## TID: %i - Solicito Contexto Ejecución", tid);
    destroy_dispatch_request(req);
    delete_package(package);
    destroy_buffer(buffer);
}

t_execution_context* receive_execution_context(int socket_cliente) {
    int size;

    void* buffer = receive_buffer(&size, socket_cliente);
    t_execution_context* execution_context = deserialize_execution_context(buffer);
    // log_error(logger, "DESERIALIZACION TID: %i, PID: %i, PC: %i", execution_context->tid, execution_context->pid, execution_context->registers->PC);
    // printf("[Recepción] Tamaño del size recibido: %d bytes\n", size);

    free(buffer);

    return execution_context;
}

void update_execution_context(t_execution_context* execution_context, int memory_fd) {
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_execution_context(execution_context, buffer);

    t_paquete *package = create_package(EXECUTION_CONTEXT_UPDATE);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);

    log_info(logger, "## TID: %i - Actualizo Contexto Ejecución", execution_context->tid);
    // log_execution_context(execution_context, "EXECUTION_CONTEXT_UPDATED");
    delete_package(package);
    char* response = receive_memory_response(memory_fd);
    log_info(logger, "Memory response for an EXECUTION CONTEXT UPDATE: %s", response);
    destroy_buffer(buffer);
    free(response);
}

