#include <cpu.h>

int *socket_server;

t_log *logger;
t_config *config;

pthread_t server_thread_interrupt;
pthread_t server_thread_dispatch;
char* puerto_server;

// Variables globales de interrupcion
int interrupcion = 0;
pthread_mutex_t mutex_interrupcion = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char* argv[]) {

    
    int init = initializer();
    if ( init == -1 ) {
        log_error(logger, "Error initializing");
        return init;
    }
    
    char* puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    log_info(logger, "Listening on port dispatch: %s", puerto_escucha_dispatch);

    char* puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    log_info(logger, "Listening on port interrupt: %s", puerto_escucha_interrupt);

    int memory_cpu_connection = connection_by_config(config, "IP_MEMORIA", "PUERTO_MEMORIA");
    log_info(logger, "Connecting to: %d", memory_cpu_connection);

    void* args[2];  
    args[0] = (void*) puerto_escucha_dispatch;
    args[1] = (void*) &memory_cpu_connection; 

    if(pthread_create(&(server_thread_dispatch), NULL, (void *) run_server_dispatch_cpu, (void *) args) != 0){
        log_error(logger, "Error creating dispatch server thread");
    };  


    if(pthread_create(&(server_thread_interrupt), NULL, (void *) run_server_interrupt, (void *) puerto_escucha_interrupt) != 0){
        log_error(logger, "Error creating interrupt server thread");
    }; 

    // Tests

    // test_dump(1, 4, memory_cpu_connection);
    // test_io(1000, memory_cpu_connection);
    // test_process_create("asdasd", 1, 2, memory_cpu_connection);
    // test_thread_create("asdasd", 2, memory_cpu_connection);
    // test_thead_operation(10, memory_cpu_connection);
    // test_mutex_operation("ASDAASDA", 3, memory_cpu_connection);
    // test_process_exit(9, memory_cpu_connection);
    // update_execution_context_test(memory_cpu_connection);
    // fetch_test(memory_cpu_connection, 1);
    // request_execution_context_test(memory_cpu_connection);
    // set_test(memory_cpu_connection, 1);
    // sum_test(memory_cpu_connection, 1);
    // sub_test(memory_cpu_connection, 1);
    // log_test(memory_cpu_connection, 1);
    // jnz_test(memory_cpu_connection, 1);
    // read_mem_test(memory_cpu_connection, 1);
    // write_mem_test(memory_cpu_connection, 1);

    pthread_join(server_thread_interrupt, NULL);
    pthread_join(server_thread_dispatch, NULL);

    // Liberar recursos después de que los hilos terminen
    free(puerto_escucha_dispatch);
    free(puerto_escucha_interrupt);

    // Liberar configuración y logger
    config_destroy(config);
    log_destroy(logger);

    return 0;
}

int initializer() {

    /* Inicializo el logger */

    // inicializar_semaforos();

    logger = log_create("cpu.log", "cpu", true, LOG_LEVEL_INFO);
    if(logger == NULL) { return -1; }

    log_info(logger, "CPU iniciado");

    /* Inicializo el config */

    char* config_path = "cpu.config";
    config = config_create(config_path);

    if (config == NULL) { 
        log_error(logger, "Error creating config");
        return -1; 
    }

    return 1;
}

void message_sending_example(int socket) {
    /* Pruebo envio de paquete */

    const char* text = "Hello from CPU!!";
    int size = strlen(text) + 1;

    t_message* message = create_message(text, size);

    t_buffer* buffer = malloc(sizeof(t_buffer));
    serialize_message(message, buffer);

    t_paquete* package_hello = create_package(MESSAGE);
    add_to_package(package_hello, buffer->stream, buffer->size);

    send_package(package_hello, socket);
    delete_package(package_hello);
}





