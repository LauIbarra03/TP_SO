#include "dispatch_cpu.h"


void handle_client_cpu_dispatch(void *arg) {
    void** client_args = (void**) arg;
    int cliente_fd = *(int*) client_args[0];
    int memory_cpu_connection = *(int*) client_args[1];

    if(cliente_fd < 0) {
        log_error(logger, "Invalid Client! Closing Connection");
        free_connection(cliente_fd);
        exit(0);
    }
    
    log_info(logger, "New client connected, socket fd: %d", cliente_fd);

    while (1) {
        int cod_op = receive_operation_code(cliente_fd);
        t_list *lista = receive_package(cliente_fd);
        void *buffer = list_get(lista, 0);

        switch (cod_op) {
            case MESSAGE:
                log_info(logger, "Message received!");
                /* t_message* mensaje = deserialize_message(buffer);
                log_info(logger, "%s", mensaje->message); */
                
                //dump_memory_test(memory_cpu_connection, cliente_fd);
                // io_test(memory_cpu_connection, cliente_fd);
                // process_create_test(memory_cpu_connection, cliente_fd);
                // thread_create_test(memory_cpu_connection, cliente_fd);
                // thread_join_test(memory_cpu_connection, cliente_fd);
                // thread_cancel_test(memory_cpu_connection, cliente_fd);
                // mutex_create_test(memory_cpu_connection, cliente_fd);
                // mutex_lock_test(memory_cpu_connection, cliente_fd);
                // mutex_unlock_test(memory_cpu_connection, cliente_fd);
                // thread_exit_test(memory_cpu_connection, cliente_fd);
                // process_exit_test(memory_cpu_connection, cliente_fd);
                 
                break;
            case DISPATCH:
                t_dispatch_request *req = deserialize_dispatch_request(buffer);
                log_warning(logger, "Dispatch TID: %i, PID: %i", req->tid, req->pid);
                request_execution_context(req->tid, req->pid, memory_cpu_connection); 
                destroy_dispatch_request(req);
                receive_operation_code(memory_cpu_connection);
                t_execution_context *execution_context = receive_execution_context(memory_cpu_connection);
                // log_execution_context(execution_context, "EXECUTION_CONTEXT_RECEIVE");
                execute_instruction_cycle(execution_context, memory_cpu_connection, cliente_fd);
                break;
                
            case DISCONNECT:
                log_info(logger, "Client disconnected. Finishing client connection.");
                list_destroy_and_destroy_elements(lista, free);
                free_connection(cliente_fd);
                return;

            default:
                log_warning(logger, "Unknown operation dispatch.");
                break;
        }
        list_destroy_and_destroy_elements(lista, free);
    }
}

void run_server_dispatch_cpu(void *arg) {
    void** args = (void**) arg;
    char* puerto_server_dispatch_cpu = (char*) args[0];
    int memory_cpu_connection = *(int*) args[1];

    int server_dispatch_cpu_fd = start_server(puerto_server_dispatch_cpu);
    log_info(logger, "Server ready to receive clients...");
    
    
    while(1) {
        int cliente_fd = wait_for_client(server_dispatch_cpu_fd);

        pthread_t client_thread;
        int *socket_server = malloc(sizeof(int));
        *socket_server = cliente_fd;

        void* client_args[2];
        client_args[0] = socket_server;
        client_args[1] = &memory_cpu_connection;

        if(pthread_create(&(client_thread), NULL, (void*) handle_client_cpu_dispatch, (void*) client_args) != 0) {
            log_error(logger, "Error creating thread for the client.");
            exit(0);
        }

        
        pthread_detach(client_thread);
    }
}


