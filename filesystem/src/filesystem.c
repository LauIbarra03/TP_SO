#include <filesystem.h>

//Librerias de commons
#include <commons/log.h>
#include <commons/config.h>

//Carpeta local utils
#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>

// Archivo de testing
#include <testing/testingFS.h>

int server_fd;
int* socket_server;

t_log *logger;
t_config *config;

pthread_t server_thread;

char* puerto_server;


/* -------------------------- DECLARACIONES DE FUNCIONES -------------------------- */
void handle_client(void *arg) {
    int cliente_fd = *(int*) arg;
    free(arg);

    if(cliente_fd < 0){
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
                t_message* mensaje = deserialize_message(buffer);
                log_info(logger, "%s", mensaje->message);
                break;

            case DUMP_MEMORY:
                log_info(logger, "DUMP MEMORY received\n");
                t_dump_context* contexto = deserialize_dump_context(buffer);

                log_debug(logger, "PID: %d, TID: %d, Size: %d", contexto->pid, contexto->tid, contexto->tamanio_proceso);

                t_estructura_para_dump* new_struct = malloc(sizeof(t_estructura_para_dump));
                new_struct->dump_context = contexto;
                new_struct->client_fd = cliente_fd;
                pthread_t new_thread;

                if(pthread_create((&new_thread), NULL, (void*) ejecutar_dump_memory, (void*)new_struct)!= 0) {
                    log_info(logger, "No se pudo crear hilo para dump");
                }

                pthread_detach(new_thread);
                
                break;
            case DISCONNECT:
                log_info(logger, "Client disconnected. Finishing client connection.");
                list_destroy_and_destroy_elements(lista, free);
                free_connection(cliente_fd);
                return;
            default:
                log_warning(logger, "Unknown operation.");
                break;
        }
        list_destroy_and_destroy_elements(lista, free);
    }
}

void run_server(void *arg) {
    puerto_server = (char *) arg;

    server_fd = start_server(puerto_server);
    log_info(logger, "Server ready to receive clients...");

    while(1) {
        int cliente_fd = wait_for_client(server_fd);

        pthread_t client_thread;
        socket_server = malloc(sizeof(int));
        *socket_server = cliente_fd;

        if(pthread_create(&(client_thread), NULL, (void*) handle_client, (void*) socket_server) != 0) {
            log_error(logger, "Error creating thread for the client.");
            exit(0);
        }

        pthread_detach(client_thread);
    }
}

/* -------------------------- MAIN -------------------------- */
int main(int argc, char* argv[]) {

    /* -------------------------- INICIALIZAR LOGGER -------------------------- */
    logger = log_create("filesystem.log", "filesystem", true, LOG_LEVEL_DEBUG);
    if (logger == NULL) { return -1; }

    log_info(logger, "File System Inicializado!");

    /* -------------------------- INICIALIZAR LISTA DE PROCESOS -------------------------- */
    // memory_process_list = list_create(); // Esta variable está declarada en mem_structs.c

    log_info(logger, "Initializing Process List!");

    /* -------------------------- INICIALIZAR CONFIG -------------------------- */
    char* config_path = "filesystem.config";
    config = config_create(config_path);

    inicializar_fs();
    //test_dump_memory();

    if (config == NULL) { 
        log_error(logger, "Error creating config");
        return -1; 
    }

    char *puerto_escucha_filesystem = config_get_string_value(config, "PUERTO_ESCUCHA");
    log_info(logger, "Listening on port %s", puerto_escucha_filesystem);    
       
    
    /* -------------------------- INICIALIZAR HILOS -------------------------- */
    if (pthread_create(&(server_thread), NULL, (void *) run_server, (void *) puerto_escucha_filesystem) != 0) {
        log_error(logger, "Error creating server thread");
        return -1;
    }

    pthread_join(server_thread, NULL); 

    return 0;
}

void ejecutar_dump_memory(void* arg) {
    t_estructura_para_dump* estructura = (t_estructura_para_dump*) arg;

    op_code codigo_retorno = execute_dump_memory(estructura->dump_context);
    
    //send_response_to_memory(codigo_retorno, estructura->client_fd);
    send_response(estructura->client_fd, codigo_retorno);
    
    free(estructura->dump_context->datos);
    free(estructura->dump_context);
    free(estructura);
}

void send_response_to_memory(op_code codigo_respuesta, int socket_cliente) {
    t_paquete *paquete = malloc(sizeof(t_paquete));

    // Configuración del paquete
    paquete->operation_code = MESSAGE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = sizeof(op_code); // Aseguramos que el tamaño sea el del enum
    paquete->buffer->stream = malloc(paquete->buffer->size);

    // Copiamos el valor del código de respuesta al buffer
    memcpy(paquete->buffer->stream, &codigo_respuesta, paquete->buffer->size);

    // Serializamos el paquete
    int bytes = paquete->buffer->size + 2 * sizeof(int); // Buffer + operación + tamaño
    void *a_enviar = serialize_package(paquete, bytes);

    // Enviamos el paquete
    send(socket_cliente, a_enviar, bytes, 0);

    // Liberamos la memoria asignada
    free(a_enviar);
    delete_package(paquete);
}

void free_estructura_para_dump(t_estructura_para_dump* estructura) {
    if (estructura == NULL) {
        return; // Si el puntero es NULL, no hacemos nada.
    }

    // Liberar el campo dinámico dump_context, si no es NULL.
    if (estructura->dump_context != NULL) {
        free(estructura->dump_context);
    }

    // Liberar la estructura principal.
    free(estructura);
}

