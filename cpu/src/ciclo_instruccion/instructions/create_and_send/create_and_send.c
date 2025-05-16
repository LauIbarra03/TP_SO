#include "create_and_send.h"

void crear_enviar_process_create(char* nombre_archivo, uint32_t tamanio_proceso, uint32_t prioridad, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = PROCESS_CREATE;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_process_create* proceso = create_syscall_process_create(nombre_archivo, tamanio_proceso, prioridad);
    if (!proceso) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_process_create(proceso, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_process_create(proceso);
    delete_package(paquete);
}

void crear_enviar_process_exit(uint32_t pid, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = PROCESS_EXIT;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_process_exit* proceso = create_syscall_process_exit(pid);
    if (!proceso) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_process_exit(proceso, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_process_exit(proceso);
    delete_package(paquete);
}

void crear_enviar_thread_create(char* nombre_archivo, uint32_t prioridad, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = THREAD_CREATE;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_thread_create* thread = create_syscall_thread_create(nombre_archivo, prioridad);
    if (!thread) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_thread_create(thread, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_thread_create(thread);
    delete_package(paquete);
}

void crear_enviar_thread_operation(op_code operation, uint32_t tid, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = operation;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_thread_operation* thread = create_syscall_thread_operation(tid);
    if (!thread) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_thread_operation(thread, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_thread_operation(thread);
    delete_package(paquete);
}

void crear_enviar_mutex_operation(op_code operation, char* nombre_semaforo, uint32_t tid, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = operation;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_mutex_operation* mutex = create_syscall_mutex_operation(nombre_semaforo, tid);
    if (!mutex) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_mutex_operation(mutex, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_mutex_operation(mutex);
    delete_package(paquete);
}

void crear_enviar_dump_memory(uint32_t pid, uint32_t tid, int cliente_fd) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = DUMP_MEMORY;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_dump_memory* dump_memory = create_dump_memory(pid, tid);
    if (!dump_memory) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_dump_memory(dump_memory, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_dump_memory(dump_memory);
    delete_package(paquete);
}

void crear_enviar_syscall_io(uint32_t milisegundos, int cliente_fd)  {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    if (!paquete) return;

    paquete->operation_code = IO;
    paquete->buffer = malloc(sizeof(t_buffer));
    if (!paquete->buffer) {
        free(paquete);
        return;
    }
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;

    t_syscall_io* syscall_io = create_syscall_io(milisegundos);
    if (!syscall_io) {
        delete_package(paquete);
        return;
    }

    // Serializar datos
    serialize_syscall_io(syscall_io, paquete->buffer);

    // Validar buffer serializado
    if (!paquete->buffer->stream || paquete->buffer->size <= 0) {
        delete_package(paquete);
        return;
    }

    // Serializar paquete completo
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    if (a_enviar) {
        // Enviar paquete
        send(cliente_fd, a_enviar, bytes, 0);
        free(a_enviar);
    }

    // Liberar recursos
    destroy_syscall_io(syscall_io);
    delete_package(paquete);
}


/* t_respuesta_syscall* recibir_respuesta_syscall(int fd_kernel) {
    t_respuesta_syscall* respuesta = malloc(sizeof(t_respuesta_syscall));
    
    ssize_t bytes_recibidos = recv(fd_kernel, respuesta, sizeof(t_respuesta_syscall), 0);

    if (bytes_recibidos <= 0) {
        perror("Error al recibir la respuesta de syscall");
        free(respuesta);
        return NULL;  // Manejo del error según sea necesario
    }

    printf("Respuesta de syscall recibida: %d\n", respuesta->codigo_respuesta);

    return respuesta;
} */

