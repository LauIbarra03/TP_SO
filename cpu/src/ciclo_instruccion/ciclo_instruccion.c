#include "ciclo_instruccion.h"


char* fetch(t_execution_context* execution_context, int memory_fd, int kernel_fd) {
    
    log_info(logger, "## TID: %i - FETCH - Program Counter: %i", execution_context->tid, execution_context->registers->PC);
    
    request_instruction_to_memory(execution_context, memory_fd); 
    // log_info(logger, "Sending INSTRUCTION_REQUEST to: %d", memory_fd);
    // log_info(logger, "EXECUTION CONTEXT, PID: %d, TID: %d, PC: %d", execution_context->pid, execution_context->tid, execution_context->registers->PC);
    receive_operation_code(memory_fd);
    char* instruction = receive_instruction(memory_fd);
    // log_info(logger, "Recieving instruction: %s", instruction);
    execution_context->registers->PC += 1;
    return instruction;
}

int decode_and_execute (char* instruction_char, t_execution_context* execution_context, int memory_fd, int kernel_fd) {
    char* param1; 
    char* param2;
    char* param3;
    uint32_t tid_int;
    uint32_t prioridad_int;
    uint32_t milisegundos_int;
    uint32_t tamanio_proceso_int;
    int read_mem_return;
    int write_mem_return;

    t_instruction *instruction = parse_instruction(instruction_char);

    for (size_t i = 0; i < instruction->params_count; i++) {
    char* current_param = (char*)list_get(instruction->params, i);

    char* temp_param = strdup(current_param); // Crear una copia del parámetro actual
    if (temp_param[strlen(temp_param) - 1] == '\n') { 
        temp_param[strlen(temp_param) - 1] = '\0'; // Reemplazar el salto de línea con '\0'
    }

    if (i == 0) {
        param1 = temp_param;
    }
    else if (i == 1) {
        param2 = temp_param;
    } else if (i == 2) {
        param3 = temp_param;
    }
}

    free(instruction_char);
    
    
    switch (instruction -> name)
    {
    case SET:
        log_info(logger, "## TID: %i - Ejecutando: SET - %s %s", execution_context->tid, param1, param2);
        
        set_instruction(param1, param2, execution_context);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return CONTINUE_INSTRUCTION;
    case SUM:
        log_info(logger, "## TID: %i - Ejecutando: SUM - %s %s", execution_context->tid, param1, param2);
        
        sum_instruction(param1, param2, execution_context);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return CONTINUE_INSTRUCTION;
    case SUB: 
        log_info(logger, "## TID: %i - Ejecutando: SUB - %s %s", execution_context->tid, param1, param2);

        sub_instruction(param1, param2, execution_context);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return CONTINUE_INSTRUCTION;

    case LOG: 
        log_info(logger, "## TID: %i - Ejecutando: LOG - %s ", execution_context->tid, param1);

        log_instruction(param1, execution_context);

        free(param1);
        destroy_instruction(instruction);

        return CONTINUE_INSTRUCTION;
    case JNZ:
        log_info(logger, "## TID: %i - Ejecutando: JNZ - %s %s", execution_context->tid, param1, param2);

        jnz_instruction(param1, param2, execution_context);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return CONTINUE_INSTRUCTION;
    case READ_MEM:
        log_info(logger, "## TID: %i - Ejecutando: READ_MEM - %s %s", execution_context->tid, param1, param2);

        read_mem_return = read_mem_instruction(param1, param2, execution_context, memory_fd, kernel_fd);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return read_mem_return;

    case WRITE_MEM:
        log_info(logger, "## TID: %i - Ejecutando: WRITE_MEM - %s %s", execution_context->tid, param1, param2);
        
        write_mem_return = write_mem_instruction(param1, param2, execution_context, memory_fd, kernel_fd);

        free(param1);
        free(param2);
        destroy_instruction(instruction);

        return write_mem_return;
    // Syscalls
    case DUMP_MEMORY:
        log_info(logger, "## TID: %i - Ejecutando: DUMP_MEMORY", execution_context->tid);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_dump_memory(execution_context->pid, execution_context->tid, kernel_fd);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case IO:        
        log_info(logger, "## TID: %i - Ejecutando: IO - %s", execution_context->tid, param1);
        milisegundos_int = casting_char_to_uint32_t(param1);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_syscall_io(milisegundos_int, kernel_fd);
        destroy_instruction(instruction);
        free(param1);
        return SYSCALL_INSTRUCTION;
    case PROCESS_CREATE:
        log_info(logger, "## TID: %i - Ejecutando: PROCESS_CREATE - %s %s %s", execution_context->tid, param1, param2, param3);
        tamanio_proceso_int = casting_char_to_uint32_t(param2);
        prioridad_int = casting_char_to_uint32_t(param3);

        update_execution_context(execution_context, memory_fd);
        
        crear_enviar_process_create(param1, tamanio_proceso_int, prioridad_int, kernel_fd);
        destroy_instruction(instruction);
        free(param1);
        free(param2);
        free(param3);
        return SYSCALL_INSTRUCTION;
    case THREAD_CREATE:
        log_info(logger, "## TID: %i - Ejecutando: THREAD_CREATE - %s %s", execution_context->tid, param1, param2);
        uint32_t prioridad_int = casting_char_to_uint32_t(param2);

        update_execution_context(execution_context, memory_fd);

        crear_enviar_thread_create(param1, prioridad_int, kernel_fd);
        destroy_instruction(instruction);
        free(param1);
        free(param2);
        return SYSCALL_INSTRUCTION;
    case THREAD_JOIN:
        log_info(logger, "## TID: %i - Ejecutando: THREAD_JOIN - %s", execution_context->tid, param1);        
        tid_int = casting_char_to_uint32_t(param1);

        update_execution_context(execution_context, memory_fd);

        crear_enviar_thread_operation(THREAD_JOIN, tid_int, kernel_fd);
        destroy_instruction(instruction);
        free(param1);
        return SYSCALL_INSTRUCTION;
    case THREAD_CANCEL:
        log_info(logger, "## TID: %i - Ejecutando: THREAD_CANCEL - %s", execution_context->tid, param1);
        tid_int = casting_char_to_uint32_t(param1);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_thread_operation(THREAD_CANCEL, tid_int, kernel_fd);
        free(param1);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case MUTEX_CREATE:
        log_info(logger, "## TID: %i - Ejecutando: MUTEX_CREATE - %s", execution_context->tid, param1);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_mutex_operation(MUTEX_CREATE, param1, execution_context->tid, kernel_fd);
        free(param1);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case MUTEX_LOCK:
        log_info(logger, "## TID: %i - Ejecutando: MUTEX_LOCK - %s", execution_context->tid, param1);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_mutex_operation(MUTEX_LOCK, param1, execution_context->tid, kernel_fd);
        free(param1);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case MUTEX_UNLOCK:
        log_info(logger, "## TID: %i - Ejecutando: MUTEX_UNLOCK - %s", execution_context->tid, param1);
        
        update_execution_context(execution_context, memory_fd);

        crear_enviar_mutex_operation(MUTEX_UNLOCK, param1, execution_context->tid, kernel_fd);
        free(param1);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case THREAD_EXIT:
        log_info(logger, "## TID: %i - Ejecutando: THREAD_EXIT", execution_context->tid);

        update_execution_context(execution_context, memory_fd);

        crear_enviar_thread_operation(THREAD_EXIT, execution_context->tid, kernel_fd);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;
    case PROCESS_EXIT:
        log_info(logger, "## TID: %i - Ejecutando: PROCESS_EXIT", execution_context->tid);

        update_execution_context(execution_context, memory_fd);
        
        crear_enviar_process_exit(execution_context->tid, kernel_fd);
        destroy_instruction(instruction);
        return SYSCALL_INSTRUCTION;       
    default:
        log_warning(logger, "Unknown instruction.");
        return -1;
    }
}


void execute_instruction_cycle(t_execution_context *execution_context, int memory_fd, int kernel_fd) {
    while (1) {

        char *instruction = fetch(execution_context, memory_fd, kernel_fd);
        int decode_value = decode_and_execute(instruction, execution_context, memory_fd, kernel_fd);

        if (decode_value == SEG_FAULT) {
            log_error(logger, "Segmentation fault, ending instruction cycle");
            send_dispatch_return(execution_context->tid, execution_context->pid, kernel_fd, SEGMENTATION_FAULT);
            destroy_execution_context(execution_context);
            break;
        }

        if (decode_value == SYSCALL_INSTRUCTION) {
            // la logica pertienente en estos casos ya la manejo en otros lados
            log_warning(logger, "Received a syscall, ending instruction cycle");
            destroy_execution_context(execution_context);
            break;
        }

        // Chequeo de interrupciones
        pthread_mutex_lock(&mutex_interrupcion);
        if (interrupcion) {
            log_warning(logger, "Received interruption, ending instruction cycle");
            
            // Resetear el flag de interrupción
            interrupcion = 0;
            pthread_mutex_unlock(&mutex_interrupcion);

            update_execution_context(execution_context, memory_fd);
            send_dispatch_return(execution_context->tid, execution_context->pid, kernel_fd, KERNEL_INTERRUPTION);
            destroy_execution_context(execution_context);
            break;  

        }   
        pthread_mutex_unlock(&mutex_interrupcion);
    }
}


// Funciones auxiliares
void send_dispatch_return(u_int32_t tid, u_int32_t pid, int kernel_fd, op_code code) {
    t_dispatch_request *request = create_dispatch_request(tid, pid);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_dispatch_request(request, buffer);

    int bytes = buffer->size + 2 * sizeof(int);
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->operation_code = code; 
    paquete->buffer = buffer;

    void *a_enviar = serialize_package(paquete, bytes);
    send(kernel_fd, a_enviar, bytes, 0);

    free(a_enviar);
    destroy_dispatch_request(request);
    delete_package(paquete);
}

char *receive_instruction(int socket_cliente) {
    int size;
    char* instruction = receive_buffer(&size, socket_cliente);
    return instruction;
}

void request_instruction_to_memory(t_execution_context* execution_context, int memory_fd) {
    u_int32_t tid = execution_context->tid;
    u_int32_t pc  = execution_context->registers->PC;
    u_int32_t pid = execution_context->pid;
    // log_warning(logger, "Desde request, TID: %d, PC: %d, PID: %d", tid, pc, pid);
    t_instruction_request *req = create_instruction_request(tid, pid, pc);
    t_buffer *buffer = malloc(sizeof(t_buffer));
    serialize_instruction_request(req, buffer);

    t_paquete *package = create_package(INSTRUCTION_REQUEST);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    destroy_instruction_request(req);
    destroy_buffer(buffer);
    delete_package(package);
}

op_code string_to_op_code(char* name) {
    if (strcmp(name, "SET") == 0) return SET;
    if (strcmp(name, "SUM") == 0) return SUM;
    if (strcmp(name, "SUB") == 0) return SUB;
    if (strcmp(name, "READ_MEM") == 0) return READ_MEM;
    if (strcmp(name, "WRITE_MEM") == 0) return WRITE_MEM;
    if (strcmp(name, "JNZ") == 0) return JNZ;
    if (strcmp(name, "LOG") == 0) return LOG;
    if (strcmp(name, "MUTEX_CREATE") == 0) return MUTEX_CREATE;
    if (strcmp(name, "MUTEX_LOCK") == 0) return MUTEX_LOCK;
    if (strcmp(name, "MUTEX_UNLOCK") == 0) return MUTEX_UNLOCK;
    if (strcmp(name, "DUMP_MEMORY") == 0) return DUMP_MEMORY;
    if (strcmp(name, "IO") == 0) return IO;
    if (strcmp(name, "PROCESS_CREATE") == 0) return PROCESS_CREATE;
    if (strcmp(name, "THREAD_CREATE") == 0) return THREAD_CREATE;
    if (strcmp(name, "THREAD_CANCEL") == 0) return THREAD_CANCEL;
    if (strcmp(name, "THREAD_JOIN") == 0) return THREAD_JOIN;
    if (strcmp(name, "THREAD_EXIT") == 0) return THREAD_EXIT;
    if (strcmp(name, "PROCESS_EXIT") == 0) return PROCESS_EXIT;
    return -1;  // Si no coincide con ninguna instrucción válida
}

void print_instruction(t_instruction* instruction) {
    
    printf("Operation: %i\n", instruction->name);
    
    // Imprimir los parámetros
    printf("Parameters (%d):\n", instruction->params_count);
    for (int i = 0; i < instruction->params_count; i++) {
        printf("  - %s\n", (char*)list_get(instruction->params, i));
    }
}

t_instruction* parse_instruction(char* input) {
    // Crear una copia de la cadena de entrada, para no modificar el original
    char* input_copy = strdup(input);
    if (input_copy == NULL) {
        return NULL; // Manejo de errores
    }

    // Crear una nueva instrucción
    t_instruction* instruction = malloc(sizeof(t_instruction));
    if (instruction == NULL) {
        free(input_copy);  // Liberar la copia en caso de error
        return NULL; // Manejo de errores
    }

    instruction->params = list_create();  // Crear la lista para los parámetros
    if (instruction->params == NULL) {
        free(instruction);
        free(input_copy);  // Liberar la copia en caso de error
        return NULL;  // Manejo de errores
    }

    // Usar strtok para dividir la cadena en partes
    char* token = strtok(input_copy, " ");
    
    // El primer token es el nombre de la operación (op_code)
    instruction->name = string_to_op_code(token);  // Convertir el string en op_code
    
    // Los siguientes tokens son los parámetros
    while ((token = strtok(NULL, " ")) != NULL) {
        char* param = strdup(token);  // Copiar el token porque strtok lo modifica
        if (param == NULL) {
            list_destroy_and_destroy_elements(instruction->params, free);  // Liberar los parámetros si hay error
            free(instruction);
            free(input_copy);  // Liberar la copia en caso de error
            return NULL;  // Manejo de errores
        }
        list_add(instruction->params, param);  // Agregar el parámetro a la lista
    }
    
    // Contar la cantidad de parámetros
    instruction->params_count = list_size(instruction->params); 
    
    free(input_copy);  // Liberar la copia de la cadena de entrada
    return instruction;
}

uint32_t casting_char_to_uint32_t(char *param) {
    char *t = param;
    uint32_t tid_int;
    tid_int = (uint32_t)strtoul(t, NULL, 10);

    return tid_int;
}


