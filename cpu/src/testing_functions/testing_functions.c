#include "testing_functions.h"

/* void seg_fault_test(int kernel_fd, int memory_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 10;
    registers->PC = 100;
    registers->BASE = 0;
    registers->LIMITE = 1;
    t_execution_context *execution_context = create_execution_context(1, 1, registers);

    translate_address(execution_context, registers->AX, memory_fd, kernel_fd);

    free(execution_context);
    free(registers); 
}

void update_execution_context_test(int memory_cpu_connection) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 10;
    registers->PC = 100;
    t_execution_context *execution_context = create_execution_context(1, 1, registers);

    update_execution_context(execution_context, memory_cpu_connection);

    free(execution_context);
    free(registers); 
} */

void request_execution_context_test(int memory_cpu_connection) {
    request_execution_context(150, 200, memory_cpu_connection);
    receive_operation_code(memory_cpu_connection);
    t_execution_context* deserialized_context = receive_execution_context(memory_cpu_connection);
    printf("Deserializado TID: %u\n", deserialized_context->tid);
    printf("Deserializado PID: %u\n", deserialized_context->pid);
    printf("Deserializado PC: %u\n", deserialized_context->registers->PC);
    printf("Deserializado AX: %u\n", deserialized_context->registers->AX);
    printf("Deserializado BX: %u\n", deserialized_context->registers->BX);
    printf("Deserializado CX: %u\n", deserialized_context->registers->CX);
    printf("Deserializado DX: %u\n", deserialized_context->registers->DX);
    printf("Deserializado EX: %u\n", deserialized_context->registers->EX);
    printf("Deserializado FX: %u\n", deserialized_context->registers->FX);
    printf("Deserializado GX: %u\n", deserialized_context->registers->GX);
    printf("Deserializado HX: %u\n", deserialized_context->registers->HX);
    printf("Deserializado BASE: %u\n", deserialized_context->registers->BASE);
    printf("Deserializado LIMITE: %u\n", deserialized_context->registers->LIMITE);
    printf("Deserializado TAMAÑO: %u\n", deserialized_context->tamanio_proceso);
    printf("Deserializado ESTADO: %u\n", deserialized_context->estado);
}

/* 
void fetch_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));

    registers->PC = 1;
    registers->AX = 100;
    registers->BX = 10;
    registers->LIMITE = 1000;
    registers->BASE = 100;
    // t_execution_context* execution_context = create_execution_context(1, 1, registers, 1);
    // fetch(execution_context, memory_fd, kernel_fd);   
}

void set_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));

    t_execution_context* execution_context = create_execution_context(2, 1, registers);
    
    decode_and_execute("SET BX 200", execution_context, memory_fd, kernel_fd);
    log_info(logger, "Valor de BX: %u", execution_context->registers->BX);

    free(execution_context);
    free(registers);
}

void sum_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 5;
    registers->BX = 10;

    t_execution_context* execution_context = create_execution_context(3, 1, registers);

    decode_and_execute("SUM AX BX", execution_context, memory_fd, kernel_fd);
    log_info(logger, "Valor de AX: %u", execution_context->registers->AX);

    free(execution_context);
    free(registers);
}

void sub_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 15; 
    registers->BX = 9; 

    t_execution_context* execution_context = create_execution_context(4, 1, registers);

    decode_and_execute("SUB AX BX", execution_context, memory_fd, kernel_fd);
    log_info(logger, "Valor de AX: %u", execution_context->registers->AX);

    free(execution_context);
    free(registers);
}

void log_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 150; 

    t_execution_context* execution_context = create_execution_context(5, 1, registers);

    decode_and_execute("LOG AX", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void jnz_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 10;
    registers->PC = 100;

    t_execution_context* execution_context = create_execution_context(6, 1, registers);
    
    log_info(logger, "Valor PC antes del JNZ: %i", execution_context->registers->PC );
    decode_and_execute("JNZ AX 15", execution_context, memory_fd, kernel_fd);
    log_info(logger, "Valor PC despues del JNZ: %i", execution_context->registers->PC );
    
    free(execution_context);
    free(registers);
}

void read_mem_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    registers->AX = 100;
    registers->BX = 10;
    registers->LIMITE = 1000;
    registers->BASE = 50;

    t_execution_context* execution_context = create_execution_context(7, 1, registers);

    decode_and_execute("READ_MEM AX BX", execution_context, memory_fd, kernel_fd);
    log_info(logger, "Valor AX despues del READ_MEM: %i", execution_context->registers->AX);

    free(execution_context);
    free(registers);
} 

void write_mem_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(8, 1, registers);
    registers->AX = 100;
    registers->BX = 10;
    registers->LIMITE = 1000;
    registers->BASE = 50;

    decode_and_execute("WRITE_MEM AX BX", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void dump_memory_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(9, 1, registers);

    decode_and_execute("DUMP_MEMORY", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void io_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(10, 1, registers);

    decode_and_execute("IO 1500", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void process_create_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(11, 1, registers);

    decode_and_execute("PROCESS_CREATE nombre_archivo 100 1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void thread_create_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(12, 1, registers);

    decode_and_execute("THREAD_CREATE asd 1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);

}

void thread_join_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(13, 1, registers);

    decode_and_execute("THREAD_JOIN 1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);

}

void thread_cancel_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(14, 1, registers);

    decode_and_execute("THREAD_CANCEL 1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void mutex_create_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(15, 1, registers);

    decode_and_execute("MUTEX_CREATE recurso_1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void mutex_lock_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(16, 1, registers);

    decode_and_execute("MUTEX_LOCK recurso_1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void mutex_unlock_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(17, 1, registers);

    decode_and_execute("MUTEX_UNLOCK recurso_1", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void thread_exit_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(18, 1, registers);

    decode_and_execute("THREAD_EXIT", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void process_exit_test(int memory_fd, int kernel_fd) {
    t_register* registers = malloc(sizeof(t_register));
    t_execution_context* execution_context = create_execution_context(19, 1, registers);

    decode_and_execute("PROCESS_EXIT", execution_context, memory_fd, kernel_fd);

    free(execution_context);
    free(registers);
}

void test_dump(u_int32_t tid, u_int32_t pid , int memory_fd) {

    t_dump_memory *req = create_dump_memory(pid, tid);
    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_dump_memory(req, buffer);
    t_paquete *package = create_package(DUMP_MEMORY);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_io(u_int32_t milisegundo, int memory_fd) {

    t_syscall_io *req = create_syscall_io(milisegundo);
    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_syscall_io(req, buffer);
    t_paquete *package = create_package(IO);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_process_create(char* nombre, uint32_t i1, uint32_t i2, int memory_fd) {
    t_process_create *p_create = create_syscall_process_create(nombre, i1, i2);

    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_process_create(p_create, buffer);
    t_paquete *package = create_package(PROCESS_CREATE);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_thread_create(char* nombre, uint32_t prio, int memory_fd) {
    t_thread_create* t = create_syscall_thread_create(nombre, prio);

    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_thread_create(t, buffer);
    t_paquete *package = create_package(THREAD_CREATE);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_thead_operation(uint32_t tid, int memory_fd) {
    t_thread_operation* t = create_syscall_thread_operation(tid);

    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_thread_operation(t, buffer);
    t_paquete *package = create_package(THREAD_JOIN);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_mutex_operation(char *nombre, uint32_t tid, int memory_fd) {
    t_mutex_operation* m = create_syscall_mutex_operation(nombre, tid);

    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_mutex_operation(m, buffer);
    t_paquete *package = create_package(MUTEX_CREATE);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

void test_process_exit(uint32_t pid, int memory_fd) {
    t_process_exit* m = create_syscall_process_exit(pid);

    t_buffer *buffer = malloc(sizeof(t_buffer));

    serialize_process_exit(m, buffer);
    t_paquete *package = create_package(PROCESS_EXIT);
    add_to_package(package, buffer->stream, buffer->size);
    send_package(package, memory_fd);
    log_info(logger, "TESTING: %i", package->operation_code);
    delete_package(package);
}

 */