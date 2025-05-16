#include "instructions.h"


// Funciones de instrucciones
void set_instruction(char *param1, char *param2, t_execution_context* execution_context) {
    u_int32_t valor_aux = strtoul(param2, NULL, 10);
    t_register_name register_aux = return_register(param1);
    
    set_value(register_aux, valor_aux, execution_context);
}

void sum_instruction(char *param1, char *param2, t_execution_context* execution_context) {
    t_register_name register_aux1 = return_register(param1);
    t_register_name register_aux2 = return_register(param2);

    u_int32_t destiny_value = get_register_value(register_aux1, execution_context);
    u_int32_t origin_value  = get_register_value(register_aux2, execution_context);

    u_int32_t result = destiny_value + origin_value;
    set_value(register_aux1, result, execution_context);
}

void sub_instruction(char *param1, char *param2, t_execution_context* execution_context) {
    t_register_name register_aux1 = return_register(param1);
    t_register_name register_aux2 = return_register(param2);
        
    u_int32_t destiny_value = get_register_value(register_aux1, execution_context);
    u_int32_t origin_value  = get_register_value(register_aux2, execution_context);
    u_int32_t result = 0;

    if (origin_value > destiny_value) {
        result = 0;
    } else {
        result = destiny_value - origin_value;
    }
        
    set_value(register_aux1, result, execution_context);
}

void log_instruction(char *param1, t_execution_context* execution_context) {
    t_register_name register_aux1 = return_register(param1);
    u_int32_t destiny_value = get_register_value(register_aux1, execution_context);
        
    log_info(logger, "## LOG - Valor del registro %s: %i", param1, destiny_value);
}

void jnz_instruction(char *param1, char* param2, t_execution_context* execution_context) {
    t_register_name register_aux1 = return_register(param1);
    u_int32_t origin_value  = get_register_value(register_aux1, execution_context);
    u_int32_t valor_aux;

    if (origin_value != 0) {
        valor_aux = strtoul(param2, NULL, 10); 
        execution_context->registers->PC = valor_aux;
    }
    else {
        log_info(logger, "El registro no tiene valor asignado");
    }
}

int write_mem_instruction(char *param1, char *param2, t_execution_context* execution_context, int memory_fd, int kernel_fd) {
    t_register_name registro_direccion = return_register(param1); // REGISTRO DIRECCION
    t_register_name registro_datos     = return_register(param2); // REGISTRO DATOS

    u_int32_t value_to_write = get_register_value(registro_datos, execution_context);     
    u_int32_t direccion_logica = get_register_value(registro_direccion, execution_context); 

    int physical_direction = translate_address(execution_context, direccion_logica, memory_fd, kernel_fd); // Direccion fisica de REGISTRO DIRECCION
    
    if (physical_direction == SEG_FAULT) {
        log_error(logger, "## TID: %i - Error: SEGMENTATION FAULT", execution_context->tid);
        return SEG_FAULT;
    }

    log_info(logger, "## TID: %i - Acción: ESCRIBIR - Dirección Física: %i", execution_context->tid, physical_direction);
        
    request_write_mem_to_memory(execution_context, physical_direction, value_to_write, memory_fd);
    char* response = receive_memory_response(memory_fd);
    log_info(logger, "Memory response for a WRITE MEM instruction: %s", response);
    free(response);
    return CONTINUE_INSTRUCTION;
}

int read_mem_instruction(char *param1, char *param2, t_execution_context* execution_context, int memory_fd, int kernel_fd) {
    t_register_name registro_datos     = return_register(param1); 
    t_register_name registro_direccion = return_register(param2); 

    u_int32_t direccion_logica = get_register_value(registro_direccion, execution_context);  

    int physical_direction = translate_address(execution_context, direccion_logica, memory_fd, kernel_fd); // Direccion Fisica del registor de Direccion

    if (physical_direction == SEG_FAULT) {
        log_error(logger, "## TID: %i - Error: SEGMENTATION FAULT", execution_context->tid);
        return SEG_FAULT;
    }

    log_info(logger, "## TID: %i - Acción: LEER - Dirección Física: %i", execution_context->tid, physical_direction);
    
    request_read_mem_to_memory(execution_context, physical_direction, memory_fd);
    receive_operation_code(memory_fd);
    uint32_t read_value = receive_read_mem_data(memory_fd); 
    log_info(logger, "Received value: %i", read_value);
    set_value(registro_datos, read_value, execution_context);
    
    return CONTINUE_INSTRUCTION;
}

// Funciones auxiliares
t_register_name return_register(char* reg) {
    
    // Remove newline character if present
    size_t len = strlen(reg);
    if (len > 0 && reg[len - 1] == '\n') {
        reg[len - 1] = '\0';
    }

    if      (strcmp(reg, "AX") == 0) return AX;
    else if (strcmp(reg, "BX") == 0) return BX;
    else if (strcmp(reg, "CX") == 0) return CX;
    else if (strcmp(reg, "DX") == 0) return DX;
    else if (strcmp(reg, "EX") == 0) return EX;
    else if (strcmp(reg, "FX") == 0) return FX;
    else if (strcmp(reg, "GX") == 0) return GX;
    else if (strcmp(reg, "HX") == 0) return HX;

    return INVALID_REGISTER; 
}

void set_value(t_register_name register_name, u_int32_t value, t_execution_context* execution_context) {
    switch (register_name) {
        case AX:
            execution_context->registers->AX = value;
            break;
        case BX:
            execution_context->registers->BX = value;
            break;
        case CX:
            execution_context->registers->CX = value;
            break;
        case DX:
            execution_context->registers->DX = value;
            break;
        case EX:
            execution_context->registers->EX = value;
            break;
        case FX:
            execution_context->registers->FX = value;
            break;
        case GX:
            execution_context->registers->GX = value;
            break;
        case HX:
            execution_context->registers->HX = value;
            break;
        case PC:
            execution_context->registers->PC = value;
            break;
        case INVALID_REGISTER:
            log_error(logger, "Registro invalido");
            break;
    }
}


uint32_t get_register_value(t_register_name register_name, t_execution_context* execution_context) {
     switch (register_name) {
        case AX:
            return execution_context->registers->AX;
            break;
        case BX:
            return execution_context->registers->BX;
            break;
        case CX:
            return execution_context->registers->CX;
            break;
        case DX:
            return execution_context->registers->DX;
            break;
        case EX:
            return execution_context->registers->EX;
            break;
        case FX:
            return execution_context->registers->FX;
            break;
        case GX:
            return execution_context->registers->GX;
            break;
        case HX:
            return execution_context->registers->HX;
            break;
        case PC:
            return execution_context->registers->PC;
            break;
        case INVALID_REGISTER:
            log_error(logger, "Registro invalido");
            return -1;
            break;
    }

    return 0;

}

void request_read_mem_to_memory(t_execution_context* execution_context, int address, int memory_fd) {
    uint32_t tid = execution_context->tid;
    uint32_t pid = execution_context->pid;
    uint32_t physical_address = address;

    t_read_mem_request *read_mem_request = create_read_mem_request(tid, pid, physical_address);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_read_mem_request(read_mem_request, buffer);

    t_paquete *package = create_package(READ_MEM_REQUEST);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    delete_package(package);
    destroy_read_mem_request(read_mem_request);
    destroy_buffer(buffer);
}

void request_write_mem_to_memory(t_execution_context* execution_context, int address, uint32_t data,int memory_fd) {
    uint32_t tid = execution_context->tid;
    uint32_t pid = execution_context->pid;
    uint32_t physical_address = address;
    uint32_t data_register_value = data;

    t_write_mem_request *write_mem_request = create_write_mem_request(tid, pid, physical_address, data_register_value);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_write_mem_request(write_mem_request, buffer);

    t_paquete *package = create_package(WRITE_MEM_REQUEST);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    destroy_write_mem_request(write_mem_request);
    delete_package(package);
    destroy_buffer(buffer);
}


uint32_t receive_read_mem_data(int socket_cliente) {
    int size;
    void* buffer = receive_buffer(&size, socket_cliente);

    uint32_t data_value;    
    memcpy(&data_value, buffer, sizeof(uint32_t));

    free(buffer);
    return data_value;
}

