#include <memoria.h>
#include <mem_structs.h> // struct t_mem_partition
#include <partitions/partitions.h>  

/* -------------------------- DECLARACIONES DE VARIABLES -------------------------- */
int server_fd;
int* socket_server;

t_log *logger;
t_config *config;

pthread_t server_thread;

char* puerto_server;

// PID autoincremental
//u_int32_t global_pid = 0;

t_list *memory_process_list;

t_list *partition_list;        // Lista de particiones
char* partitioning_system;     // Fija o Dinamica
char* partitioning_criteria;   // First, best fit etc

uint32_t TAM_MEMORIA;
void* memoria_usuario;

int tiempo_retardo_peticiones;

/* -------------------------- DECLARACIONES DE FUNCIONES -------------------------- */
void handle_client(void *arg) {
    int cliente_fd = *(int*) arg;
    free(arg);

    if(cliente_fd < 0) {
        log_error(logger, "Invalid Client! Closing Connection");
        free_connection(cliente_fd);
        exit(0);
    }
    if (cliente_fd == 5)
    {
        log_warning(logger, "## CPU Conectado - FD del socket: %i", cliente_fd);
    } else {
        log_warning(logger, "## Kernel Conectado - FD del socket: %i", cliente_fd);
    }
    
    while (1) {
        // log_warning(logger, "Ejecutando recv package");
        // print_partitions_summary(partition_list);
        int cod_op = receive_operation_code(cliente_fd);
        t_list *lista = receive_package(cliente_fd);
        if (list_is_empty(lista)) {
            // log_error(logger, "La lista recibida está vacía. Saliendo del ciclo.");
            list_destroy(lista);  // Destruir la lista si está vacía
            break;  // Salir si la lista está vacía
        }
        void *buffer = list_get(lista, 0);

        switch (cod_op) {
            case DUMP_MEMORY:

                //log_memory_process_list();

                t_dump_memory* dump = deserialize_dump_memory(buffer);
                log_info(logger, "## Memory Dump solicitado - (PID:TID) - (%i:%i)", dump->pid, dump->tid);
                t_mem_pcb* pcb_mem_dump = buscar_proceso_por_pid(dump->pid);
                void* datos_pcb = obtener_memoria_proceso(dump->pid);
                t_dump_context* context = create_dump_context(dump->pid, dump->tid, pcb_mem_dump->size, datos_pcb);
                op_code response = send_dump_memory(context);
                log_warning(logger, "Respuesta enviada al kernel: %d", response);
                enviar_respuesta_ante_peticion(response, cliente_fd);
                // free_mem_pcb(pcb_mem_dump); // ayayaay
                destroy_dump_memory(dump);
                //log_memory_process_list();  
                // print_partitions_summary(partition_list);
            break;

            case PROCESS_CREATE: 
                // log_info(logger, "Process creation request received!"); 
            
                // t_pcb *pcb = deserialize_pcb(buffer);
                t_pcb_request *pcb_request = deserialize_pcb_request(buffer);

                const char* respuesta1 = verificar_espacio_y_crear_proceso(pcb_request->PID, pcb_request->size);

                if (strcmp(respuesta1, "Ok") == 0) {

                    
                    //print_partitions(partition_list);
                    
                    log_info(logger, "## Proceso Creado - PID: %i - Tamaño: %i", pcb_request->PID, pcb_request->size);
                    enviar_respuesta_ante_peticion(OK, cliente_fd);
                    
                } else {
                    log_warning(logger, "No hay suficiente espacio para crear el proceso %d.", pcb_request->PID);
                    enviar_respuesta_ante_peticion(ERROR, cliente_fd);
                }
                destroy_pcb_request(pcb_request);
                print_partitions_summary(partition_list);
                break;
            case THREAD_CREATE:
                // log_info(logger, "Thread creation request received!");
                t_tcb *tcb = deserialize_tcb(buffer);

                const char* respuesta2 = verificar_y_agregar_hilo(tcb->parent_PID, tcb->TID, tcb->file_path);
                if (strcmp(respuesta2, "Ok") == 0) {
                    log_info(logger, "## Hilo Creado - (PID:TID) - (%i:%i)", tcb->parent_PID, tcb->TID);
                    enviar_respuesta_ante_peticion(OK, cliente_fd);
                } else {
                    log_warning(logger, "No se pudo crear el hilo %d en el proceso %d.", tcb->TID, tcb->parent_PID);
                    enviar_respuesta_ante_peticion(ERROR, cliente_fd);
                }

                delete_tcb(tcb);      
                print_partitions_summary(partition_list);          
                break;

            case INSTRUCTION_REQUEST:
                // log_info(logger, "Instruction request received!");
                t_instruction_request *instruction_request = deserialize_instruction_request(buffer);
                // Este log no es necesario es para ver si los datos llegaron bien o no
                // log_info(logger, "Intruction request data received, TID: %i, PC: %i, PID: %i", instruction_request->tid, instruction_request->pc, instruction_request->pid);
                usleep(tiempo_retardo_peticiones * 1000);
                
                char* linea = obtener_linea_pseudocodigo(instruction_request->pid, instruction_request->tid, instruction_request->pc);
                if (linea) {
                    linea[strcspn(linea, "\n")] = '\0'; // Corta la línea donde aparece '\n'
                }
                if(linea != NULL) {
                    log_info(logger, "## Obtener instrucción - (PID:TID) - (%i:%i) - Instrucción: %s", instruction_request->pid, instruction_request->tid, linea);
                    send_instruction(linea, cliente_fd);
                }
                else {
                    log_info(logger, "No se encontro la instruccion pedida.\n");
                    enviar_respuesta_ante_peticion(ERROR, cliente_fd);
                }
                free(linea);
                free(instruction_request);
                break;

            case EXECUTION_CONTEXT:
                t_dispatch_request *req = deserialize_dispatch_request(buffer);
                log_info(logger, "## Contexto Solicitado - (PID:TID) - (%i:%i)", req->pid, req->tid);
                // log_info(logger, "Received context execution request, PID: %i, TID: %i", req->pid, req->tid);
                usleep(tiempo_retardo_peticiones * 1000);

                t_register* registers = obtener_registros_tcb(req->pid, req->tid);

                t_mem_pcb* pcb_mem = buscar_proceso_por_pid(req->pid);
                uint32_t tam_proceso = abs(registers->LIMITE - registers->BASE);
                t_execution_context* execution_context = create_execution_context(req->tid, req->pid, registers, RUNNING, tam_proceso);
                // log_warning(logger, "TID: %i, PID: %i, PC: %i", execution_context->tid, execution_context->pid, execution_context->registers->PC);
                // log_execution_context(execution_context, "EXECUTION_CONTEXT_REQUEST");
                send_execution_context(execution_context, cliente_fd);
                destroy_execution_context(execution_context);
                destroy_dispatch_request(req);
                break;

            case EXECUTION_CONTEXT_UPDATE:
                // log_info(logger, "Execution context update request received!");
                t_execution_context *new_execution_context = deserialize_execution_context(buffer);
                // log_info(logger, "Received context execution update, PID: %i, TID: %i", new_execution_context->pid, new_execution_context->tid);
                usleep(tiempo_retardo_peticiones * 1000);
                actualizar_registros(new_execution_context->pid, new_execution_context->tid, new_execution_context->registers);
                // log_execution_context(new_execution_context, "EXECUTION_CONTEXT_UPDATE");
                log_info(logger, "## Contexto Actualizado - (PID:TID) - (%i:%i)", new_execution_context->pid, new_execution_context->tid);
                send_response_to_cpu("ok", cliente_fd);
                destroy_execution_context(new_execution_context);
                break;

            case READ_MEM_REQUEST:
                // log_info(logger, "received READ MEM request");
                t_read_mem_request *read_mem_request = deserialize_read_mem_request(buffer);
                log_info(logger, "## Lectura - (PID:TID) - (%i:%i) - Dir. Física: %i - Tamaño: 4", read_mem_request->pid, read_mem_request->tid, read_mem_request->physical_address);
                usleep(tiempo_retardo_peticiones * 1000);
                uint32_t data_value = leer_uint32_t_en_buffer(read_mem_request->physical_address, memoria_usuario);
                log_info(logger, "sending value: %i", data_value);
                send_read_mem_data(data_value, cliente_fd);
                destroy_read_mem_request(read_mem_request);
                break;

            case WRITE_MEM_REQUEST:
                // log_info(logger, "received WRITE MEM request");
                t_write_mem_request *write_mem_request = deserialize_write_mem_request(buffer);
                log_info(logger, "## Escritura - (PID:TID) - (%i:%i) - Dir. Física: %i - Tamaño: 4", write_mem_request->pid, write_mem_request->tid, write_mem_request->physical_address);
                usleep(tiempo_retardo_peticiones * 1000);
                escribir_en_memoria(write_mem_request->data_register_value, memoria_usuario, write_mem_request->physical_address);
                send_response_to_cpu("ok", cliente_fd);
                destroy_write_mem_request(write_mem_request);
                break;

            case PROCESS_EXIT: {
                // log_info(logger, "Process exit request received!");
                // t_pcb *pcb = deserialize_pcb(buffer);
                t_pcb_request *pcb_request = deserialize_pcb_request(buffer);
                // Llamar a la función para eliminar el proceso
                eliminar_proceso_mem(pcb_request->PID);

                log_info(logger, "## Proceso Destruido - PID: %i - Tamaño: %i", pcb_request->PID, pcb_request->size);
                enviar_respuesta_ante_peticion(OK, cliente_fd);

                destroy_pcb_request(pcb_request);
                print_partitions_summary(partition_list);
                break;
            }

            case THREAD_EXIT: {
                // log_info(logger, "Thread exit request received!");

                t_tcb *exit_request = deserialize_tcb(buffer);

                uint32_t tid = exit_request->TID;
                uint32_t ppid = exit_request->parent_PID;

                // Llamar a la función para eliminar el hilo del proceso
                
                eliminar_hilo_mem(ppid, tid);
                // log_debug(logger, "Hilo %d eliminado correctamente del proceso %d.", tid, ppid);
                enviar_respuesta_ante_peticion(OK, cliente_fd);

                // Liberar la memoria de la solicitud deserializada
                delete_tcb(exit_request);
                print_partitions_summary(partition_list);
                break;
            }

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
        // print_partitions_summary(partition_list);
    }
}

void run_server(void *arg) {
    puerto_server = (char *) arg;

    server_fd = start_server(puerto_server);
    log_info(logger, "Server ready to receive clients...");

    while (1) {
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

void destroy_string_array(char** array) {
    if (array == NULL) return;
    string_iterate_lines(array, (void*) free); // Libera cada string del array.
    free(array); // Libera el array de punteros.
}


int main(int argc, char* argv[]) {
    /* -------------------------- INICIALIZAR LOGGER -------------------------- */
    logger = log_create("memoria.log", "memoria", true, LOG_LEVEL_DEBUG);
    if (logger == NULL) { return -1; }

    log_info(logger, "Initializing Memory!");

    /* -------------------------- INICIALIZAR LISTAS -------------------------- */
    log_info(logger, "Initializing Memory Lists");
    memory_process_list = list_create();

    /* -------------------------- INICIALIZAR CONFIG -------------------------- */
    char* config_path = "memoria.config";
    config = config_create(config_path);

    if (config == NULL) { 
        log_error(logger, "Error creating config");
        return -1; 
    }

    partitioning_system = config_get_string_value(config, "ESQUEMA");
    log_info(logger, "Partitioning System: %s", partitioning_system);

    int tamanio_memoria_config = config_get_int_value(config, "TAM_MEMORIA");
    TAM_MEMORIA = (uint32_t) tamanio_memoria_config;
    inicializar_memoria();

    tiempo_retardo_peticiones = config_get_int_value(config, "RETARDO_RESPUESTA");

    // Criterio de búsqueda
    partitioning_criteria = config_get_string_value(config, "ALGORITMO_BUSQUEDA");
    log_info(logger, "Search Algorithm: %s", partitioning_criteria); 

    /* -------------------------- INICIALIZAR PARTICIONES -------------------------- */

    if (strcmp(partitioning_system, "FIJAS") == 0) {

        char** partitions = config_get_array_value(config, "PARTICIONES");  // Crea el array de Strings para inicializar particiones estáticas
        partition_list = build_static_partition_list(partitions);           // Crea la lista de particiones estáticas
        destroy_string_array(partitions);                                   // Destruye el array de strings una vez que no se necesita más

        log_info(logger, "Static partitions initialized successfully.");

    } else {

        partition_list = build_dynamic_partition_list();
        log_info(logger, "Dynamic partition system initialized successfully.");

    } 

    
    /* -------------------------- INICIALIZAR HILOS -------------------------- */
    char *puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    log_info(logger, "Listening on port %s", puerto);

    

    if (pthread_create(&(server_thread), NULL, (void *) run_server, (void *) puerto) != 0) {
        log_error(logger, "Error creating server thread");
        return -1;
    }

    pthread_join(server_thread, NULL);

    config_destroy(config);
    log_destroy(logger);

    // Liberar recursos
    free_partition_list(partition_list);
    list_destroy(memory_process_list);

    return 0;
}

void send_response_to_cpu(char *mensaje, int socket_cliente) {
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->operation_code = MESSAGE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serialize_package(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    delete_package(paquete);
}

op_code send_dump_memory(t_dump_context* context) {

    t_buffer* buffer = malloc(sizeof(t_buffer));
    serialize_dump_context(context, buffer);

    t_paquete* paquete = create_package(DUMP_MEMORY);

    add_to_package(paquete, buffer->stream, buffer->size);
    
    int server_file_system = connection_by_config(config, "IP_MEMORY", "PUERTO_FILESYSTEM");

    send_package(paquete, server_file_system);

    op_code response = wait_for_response(server_file_system);

    send_response(server_file_system, DISCONNECT);

    free(context->datos); // Liberar la memoria asignada a 'datos' si es necesario
    free(context);         // Liberar la memoria de la estructura 't_dump_context'
    delete_package(paquete);
    destroy_buffer(buffer);
    return response;
}


char *receive_fs_response(int socket_cliente) {
    receive_operation_code(socket_cliente);
    int size;
    char* instruction = receive_buffer(&size, socket_cliente);
    return instruction;
}

