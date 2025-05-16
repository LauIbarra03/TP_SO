#include "interrupt_cpu.h"

void handle_client_interrupt(void *arg) {
    int cliente_interrupt_fd = *(int*) arg;
    free(arg);

    if (cliente_interrupt_fd < 0) {
        log_error(logger, "Invalid Client! Closing Connection");
        free_connection(cliente_interrupt_fd);
        exit(0);
    }
    
    log_info(logger, "New client connected, socket interrupt: %d", cliente_interrupt_fd);
    
    while (1) {
        // log_info(logger, "Estoy esperando un opcode");

        int cod_op = receive_operation_code(cliente_interrupt_fd);
        //t_list *lista = receive_package(cliente_interrupt_fd);

        //void *buffer = list_get(lista, 0);

        switch (cod_op) {
            case MESSAGE:
                log_info(logger, "Message received!");
                //t_message* mensaje = deserialize_message(buffer);
                //log_info(logger, "%s", mensaje->message);
                break;
            case DISCONNECT:
                log_info(logger, "Client disconnected. Finishing client connection.");
                //list_destroy_and_destroy_elements(lista, free);
                free_connection(cliente_interrupt_fd);
                return;
                
            case QUANTUM_INTERRUPTION:
            case KERNEL_INTERRUPTION:
                log_info(logger, "## Llega interrupción al puerto Interrupt");
                
                // Sincronizar el flag de interrupción
                pthread_mutex_lock(&mutex_interrupcion);
                interrupcion = 1;  // Se marca que hay una interrupción
                pthread_mutex_unlock(&mutex_interrupcion);

                // log_info(logger, "NO estoy trabado");
                break;

            default:
                log_warning(logger, "Unknown operation interrupt.");
                break;
        }
        //list_destroy_and_destroy_elements(lista, free);
    }
    
}

void run_server_interrupt(void *arg) {
    char* puerto_server_interrupt = (char *) arg;

    int server_interrupt_fd = start_server(puerto_server_interrupt);
    log_info(logger, "Server interrupt ready to receive clients...");

    

    while (1) {
        int cliente_interrupt_fd = wait_for_client(server_interrupt_fd);
        
        pthread_t client_thread_interrupt;
        int *socket_server_interrupt = malloc(sizeof(int));
        *socket_server_interrupt = cliente_interrupt_fd;


        if (pthread_create(&(client_thread_interrupt), NULL, (void*) handle_client_interrupt, (void*) socket_server_interrupt)!= 0 )   {
            log_error(logger, "Error creating thread for the client.");
            exit(0);
        }

        pthread_detach(client_thread_interrupt);
    }
}
