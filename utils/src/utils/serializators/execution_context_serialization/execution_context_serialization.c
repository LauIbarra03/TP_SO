#include "execution_context_serialization.h"

t_execution_context* create_execution_context(u_int32_t tid, u_int32_t pid, t_register* registers, t_state estado, uint32_t tamanio_proceso) {
    // Reservamos memoria para el contexto de ejecución
    t_execution_context *execution_context = malloc(sizeof(t_execution_context));

    if (execution_context == NULL) {
        destroy_execution_context(execution_context);
        return NULL; // Si la asignación falla, retornamos NULL
    }

    // Asignamos los valores del contexto
    execution_context->tid = tid;
    execution_context->pid = pid;
    execution_context->registers = registers;
    execution_context->estado = estado;
    execution_context->tamanio_proceso = tamanio_proceso; // Asignamos el tamaño del proceso

    return execution_context;
}

void serialize_execution_context(t_execution_context* execution_context, t_buffer* buffer) {
    buffer->offset = 0;

    size_t size = sizeof(uint32_t) * 3 // tid, pid, tamanio_proceso
                + sizeof(uint32_t)    // estado
                + sizeof(t_register); // registers
    buffer->size = size;

    buffer->stream = malloc(size);
    // printf("Tamaño de t_register: %zu bytes\n", sizeof(t_register));
    // Log: Antes de serializar
    /* printf("Serializando contexto de ejecución:\n");
    printf("  TID: %u\n", execution_context->tid);
    printf("  PID: %u\n", execution_context->pid);
    printf("  Estado: %d\n", execution_context->estado);
    printf("  Tamaño del proceso: %u\n", execution_context->tamanio_proceso);
    printf("  Registros: PC=%u, AX=%u, BX=%u, ...\n",
           execution_context->registers->PC,
           execution_context->registers->AX,
           execution_context->registers->BX); */

    // Serializamos los datos
    memcpy(buffer->stream + buffer->offset, &(execution_context->tid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, &(execution_context->pid), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    uint32_t estado_serializado = (uint32_t)execution_context->estado;
    memcpy(buffer->stream + buffer->offset, &estado_serializado, sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, &(execution_context->tamanio_proceso), sizeof(uint32_t));
    buffer->offset += sizeof(uint32_t);

    memcpy(buffer->stream + buffer->offset, execution_context->registers, sizeof(t_register));
    buffer->offset += sizeof(t_register);

    // Log: Después de serializar
    // printf("Serialización completa. Tamaño total: %zu bytes\n", buffer->size);
}

t_execution_context* deserialize_execution_context(void* stream) {
    t_execution_context* execution_context = malloc(sizeof(t_execution_context));
    int offset = 0;

    memcpy(&(execution_context->tid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&(execution_context->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint32_t estado_serializado;
    memcpy(&estado_serializado, stream + offset, sizeof(uint32_t));
    execution_context->estado = (t_state)estado_serializado;
    offset += sizeof(uint32_t);

    memcpy(&(execution_context->tamanio_proceso), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    execution_context->registers = malloc(sizeof(t_register));
    memcpy(execution_context->registers, stream + offset, sizeof(t_register));
    offset += sizeof(t_register);

    // Log: Después de deserializar
    /* printf("Deserialización completa:\n");
    printf("  TID: %u\n", execution_context->tid);
    printf("  PID: %u\n", execution_context->pid);
    printf("  Estado: %d\n", execution_context->estado);
    printf("  Tamaño del proceso: %u\n", execution_context->tamanio_proceso);
    printf("  Registros: PC=%u, AX=%u, BX=%u, ...\n",
           execution_context->registers->PC,
           execution_context->registers->AX,
           execution_context->registers->BX);  */
    // printf("[Deserializar] Tamaño esperado: %d bytes\n", (sizeof(t_execution_context)));
    return execution_context;
}

void destroy_execution_context(t_execution_context* execution_context) {
    if (execution_context != NULL) {
        if (execution_context->registers != NULL) {
            free(execution_context->registers);
        }
        free(execution_context);
    }
}

t_dump_context* create_dump_context(uint32_t pid, uint32_t tid,uint32_t process_size, void* datos) {
    // Reservamos memoria para el contexto de dump
    t_dump_context* dump_context = malloc(sizeof(t_dump_context));

    if (dump_context == NULL) {
        return NULL; // Si la asignación falla, retornamos NULL
    }

    // Asignamos los valores al struct
    dump_context->tid = tid;
    dump_context->pid = pid;
    dump_context->tamanio_proceso = process_size;
    dump_context->datos = datos;

    return dump_context;
}

void serialize_dump_context(t_dump_context* dump_context, t_buffer* buffer) {
    // Calcula el tamaño total de la estructura serializada
    buffer->size = 0;

    // Tamaño de los campos individuales (estáticos)
    buffer->size += sizeof(dump_context->pid);              // PID
    buffer->size += sizeof(dump_context->tid);              // TID
    buffer->size += sizeof(dump_context->tamanio_proceso);  // Tamaño del proceso

    // Tamaño de los datos dinámicos
    buffer->size += dump_context->tamanio_proceso;          // Datos apuntados por `void*`

    // Reserva memoria para el stream
    buffer->stream = malloc(buffer->size);
    if (buffer->stream == NULL) {
        log_error(logger, "Error al reservar memoria para buffer->stream");
        return;
    }

    // Serialización real (como ya tienes implementada)
    size_t offset = 0;
    memcpy(buffer->stream + offset, &dump_context->pid, sizeof(dump_context->pid));
    offset += sizeof(dump_context->pid);

    memcpy(buffer->stream + offset, &dump_context->tid, sizeof(dump_context->tid));
    offset += sizeof(dump_context->tid);

    memcpy(buffer->stream + offset, &dump_context->tamanio_proceso, sizeof(dump_context->tamanio_proceso));
    offset += sizeof(dump_context->tamanio_proceso);

    memcpy(buffer->stream + offset, dump_context->datos, dump_context->tamanio_proceso);
}


t_dump_context* deserialize_dump_context(void* stream) {
    t_dump_context* dump_context = malloc(sizeof(t_dump_context));
    int offset = 0;

    // Deserializar los datos
    memcpy(&(dump_context->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&(dump_context->tid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&(dump_context->tamanio_proceso), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    // Reservamos memoria para el campo 'datos' de tipo void* basado en el tamaño del proceso
    dump_context->datos = malloc(dump_context->tamanio_proceso);

    if (dump_context->datos != NULL) {
        // Deserializamos los datos en el puntero 'datos'
        memcpy(dump_context->datos, stream + offset, dump_context->tamanio_proceso);
    } else {
        // Si la reserva de memoria para 'datos' falla, debemos manejar el error adecuadamente
        free(dump_context); // Liberamos la memoria de dump_context
        return NULL; // Retornamos NULL en caso de error
    }

    return dump_context;
}


void destroy_dump_context(t_dump_context* context) {
    if (context == NULL) {
        return;
    }

    // Liberar memoria asignada a 'datos', si corresponde
    if (context->datos != NULL) {
        free(context->datos);
        context->datos = NULL;  // Evitar punteros colgantes
    }

    // Liberar el propio objeto 'context', si fue dinámicamente asignado
    free(context);
}


void log_execution_context(t_execution_context* execution_context, char* type) {
    log_warning(logger, "%s", type);
    log_warning(logger, "PID: %i", execution_context->pid);
    log_warning(logger, "TID: %i", execution_context->tid);
    log_warning(logger, "REGISTER PC: %i", execution_context->registers->PC);
    log_warning(logger, "REGISTER AX: %i", execution_context->registers->AX);
    log_warning(logger, "REGISTER BX: %i", execution_context->registers->BX);
    log_warning(logger, "REGISTER CX: %i", execution_context->registers->CX);
    log_warning(logger, "REGISTER DX: %i", execution_context->registers->DX);
    log_warning(logger, "REGISTER EX: %i", execution_context->registers->EX);
    log_warning(logger, "REGISTER FX: %i", execution_context->registers->FX);
    log_warning(logger, "REGISTER GX: %i", execution_context->registers->GX);
    log_warning(logger, "REGISTER HX: %i", execution_context->registers->HX);
    log_warning(logger, "REGISTER LIMITE: %i", execution_context->registers->LIMITE);
    log_warning(logger, "REGISTER BASE: %i", execution_context->registers->BASE);
}

