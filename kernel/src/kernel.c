//Librerias de C
#include <pthread.h>
#include <semaphore.h>

//Librerias de commons
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

//Carpeta local utils
#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>

//Carpeta local kernel
#include <process.h>
#include <pcb_lists.h>
#include <tcb_lists.h>
#include <scheduler.h>
#include <io_device/io_device.h>
#include <quantum_timer.h>
#include <kernel.h>
#include <mutexes/mutex_thread.h>
#include <join_thread.h>

#include <long_term_list.h>

#include <utils/serializators/syscalls_serialization/process_create_serialization/process_create_serialization.h>

/* -------------------------- DECLARACIONES DE VARIABLES -------------------------- */
int server_fd;
int* socket_server;

//Conexiones
int socket_memoria_kernel;
int quantum_interruption = 0;
int kernel_interruption = 0;

t_list *list_first_thread_params;

sem_t mutex_kernel_interruption;
sem_t mutex_quantum_interruption;
sem_t sincro_interrupcion;

sem_t sem_thread_count;

t_log *logger;
t_config *config;

pthread_t server_thread;
pthread_t io_thread; 

char* puerto_server;

//Pid Autoincremental de los pcb
//extern u_int32_t global_pid = 0;

int server_fd;
int* socket_server;

pthread_t connection_dispatch_thread;
pthread_t connection_interrupt_thread;
pthread_t quantum_thread;

//Configuracion
char* scheduler_algorithm;
int scheduler_quantum;

//char* puerto_server;


/* -------------------------- DEFINICION DE FUNCIONES -------------------------- */

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
                free(mensaje->message);
                free(mensaje);
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
    char* puerto_server = (char *) arg;

    server_fd = start_server(puerto_server);
    log_info(logger, "Server ready to receive clients...");

    while(1) {
        int cliente_fd = wait_for_client(server_fd);

        if(cliente_fd < 0) {
            log_error(logger, "Error accepting a client connection");
            continue;
        }

        pthread_t client_thread;
        socket_server = malloc(sizeof(int));

        if (socket_server == NULL) {
            log_error(logger, "Memory allocation failed for client socket.");
            free_connection(cliente_fd); // Cerrar la conexión con el cliente
            continue;
        }

        *socket_server = cliente_fd;

        if(pthread_create(&(client_thread), NULL, (void*) handle_client, (void*) socket_server) != 0) {
            log_error(logger, "Error creating thread for the client.");
            free(socket_server);
            free_connection(cliente_fd);
            continue;
        }
        pthread_detach(client_thread);
    }
}

void connect_cpu_dispatch(void* arg) {
    t_config* config = (t_config*) arg;

    pthread_t thread_client_cpu;

    if(pthread_create(&(thread_client_cpu), NULL, (void*) enviar_pcb_y_esperar_respuesta, config) != 0) {
        log_error(logger, "Error creting thread for connecting cpu. ");
        free(config);
        exit(0);
    }

    pthread_join(thread_client_cpu, NULL);

    free(config);
}

void connect_cpu_interrupt(void* arg) {

    // log_warning(logger, "\n***********SE INICIALIZA CONNECT_CPU_INTERRUPT***********\n");
    
    t_config* config = (t_config*) arg;

    pthread_t thread_client_cpu;

    if(pthread_create(&(thread_client_cpu), NULL, (void*) interrupt_cpu, config) != 0) {
        log_error(logger, "Error creting thread for connecting cpu. ");
        free(config);
        exit(0);
    }

    pthread_detach(thread_client_cpu);
}

/* -------------------------- MAIN -------------------------- */
int main(int argc, char* argv[]) {
    
    /* -------------------------- INICIALIZAR LOGGER -------------------------- */
    logger = log_create("kernel.log", "kernel", true, LOG_LEVEL_DEBUG);
    if (logger == NULL) { return -1; }

    log_info(logger, "Logger de Kernel inicializado");

    /* -------------------------- INICIALIZAR SCHEDULER -------------------------- */
    init_pcb_lists();
    init_tcb_lists();
    init_scheduler_mutexes();               
    init_quantum_timer_variables();
    init_io_queue();
    init_long_term_scheduler();

    sem_init(&mutex_kernel_interruption, 0, 1);
    sem_init(&mutex_quantum_interruption, 0, 1);
    sem_init(&sincro_interrupcion, 0, 0);

    init_tcb_joins();
    //init_mutex_waitlist();

    /* -------------------------- INICIALIZAR SEMAFORO DE HILOS -------------------------- */
    sem_init(&sem_thread_count, 0, 0);
    log_info(logger, "Initialized thread counter semaphore");

    /* -------------------------- INICIALIZAR CONFIG -------------------------- */
    char* config_path = "kernel.config";
    config = config_create(config_path);

    if (config == NULL) { 
        log_error(logger, "Error creating config");
        free(config_path);  // Liberar config_path ya que no se usa más
        return -1; 
    }


    scheduler_algorithm = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    scheduler_quantum = config_get_int_value(config, "QUANTUM");

    log_info(logger, "Inicializando planificación de tipo %s y quantum %d", scheduler_algorithm, scheduler_quantum);

    
    /* -------------------------- INICIALIZAR HILOS -------------------------- */

    if(pthread_create(&(connection_dispatch_thread), NULL, (void*) connect_cpu_dispatch, (void*) config) != 0) {
        log_error(logger, "Error creating connection thread");
        free(config);
        free(config_path);
        return -1;
    }

    if(pthread_create(&io_thread, NULL, io_device_thread, NULL) != 0) {
        log_error(logger, "Error al crear hilo de IO");
        free(config);
        free(config_path);
        return -1;
    }
    
    if(pthread_create(&(connection_interrupt_thread), NULL, (void*) connect_cpu_interrupt, (void*) config) != 0) {
        log_error(logger, "Error creatin connection thread");
        free(config);
        free(config_path);
        return -1;
    }

    if (pthread_create(&quantum_thread, NULL, run_quantum_counter, &scheduler_quantum) != 0) {
        log_error(logger, "Error creando el hilo de quantum");
        free(config);
        free(config_path);
        return -1;
    }

    /* -------------------------- PROBAR CREACION PCB -------------------------- */
    
    
    /* -------------------------- INICIALIZAR PRIMER PROCESO Y HILO 0 -------------------------- */

    char* nombre_archivo = argv[1];  // El nombre del archivo se pasa como primer argumento
    uint32_t tam_proceso = (uint32_t)atoi(argv[2]);  // El tamaño del proceso se pasa como segundo argumento
    // log_debug(logger, "Tamaño: %i, name: %s", tam_proceso, nombre_archivo);

    create_process(tam_proceso, nombre_archivo, 0);

    t_process_create* params_primer_pcb = create_syscall_process_create(nombre_archivo, tam_proceso, 0);
    list_add(list_first_thread_params, params_primer_pcb);

    try_create_NEW_process_loop();

    pthread_join(quantum_thread, NULL);
    pthread_join(connection_dispatch_thread, NULL);
    pthread_join(io_thread, NULL);
    pthread_join(connection_interrupt_thread, NULL); 

    config_destroy(config);
    log_destroy(logger);

    return 0;
}



