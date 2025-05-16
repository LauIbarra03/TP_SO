#include "mem_structs.h"
#include "partitions/partitions.h"
#include <utils/communication.h>
#include <stdlib.h>
#include <string.h>

extern t_log* logger;

extern t_list* memory_process_list;
extern t_list *partition_list;
extern char* partitioning_system;
extern char* partitioning_criteria;

typedef struct{
     uint32_t base;      
    uint32_t limite;    
    bool ocupada;       
    uint32_t pid;       
} t_particion;
t_particion* lista_particiones;

typedef struct{
    uint32_t tamanio_linea;
} t_linea_pseudocodigo;

void init_registers(t_mem_register* regs) {
    regs->PC = 0;
    regs->AX = 0;
    regs->BX = 0;
    regs->CX = 0;
    regs->DX = 0;
    regs->EX = 0;
    regs->FX = 0;
    regs->GX = 0;
    regs->HX = 0;
}

// Función para crear un proceso (PCB) y agregarlo a la lista global de procesos en memoria
t_mem_pcb* crear_proceso_mem(uint32_t pid, uint32_t base, uint32_t limite, uint32_t size) {
    // Crear un nuevo proceso
    t_mem_pcb* new_process = malloc(sizeof(t_mem_pcb));
    new_process->PID = pid;
    new_process->base = base;
    new_process->limite = limite;
    new_process->size = size;
    init_registers(&(new_process->regs)); // Inicializar los registros del proceso
    new_process->threads = list_create(); // Inicializar la lista de hilos

    // Agregar el proceso a la lista global de procesos en memoria
    list_add(memory_process_list, new_process);

    // Loggear la creación del proceso
    /*
    log_info(logger, "Proceso creado y agregado a la lista: PID=%d, Base=0x%x, Limite=0x%x, Tamaño=%d bytes", 
             pid, base, limite, size);
    */

    return new_process;
}

void* obtener_memoria_proceso(uint32_t pid) {
    t_mem_pcb* pcb = buscar_proceso_por_pid(pid);
    
    // Verificar argumentos
    if (pcb == NULL || memoria_usuario == NULL) {
        log_error(logger, "Error: PCB o memoria_usuario son NULL.\n");
        return NULL;
    }

    // Calcular el tamaño de la memoria utilizada por el proceso en bytes
    uint32_t memoria_usada = pcb->limite - pcb->base;

    // Verificar que los límites son válidos
    if (memoria_usada > pcb->size) {
        log_error(logger, "Error: La memoria utilizada excede el tamaño asignado.\n");
        return NULL;
    }

    // Reservar memoria dinámica para copiar los datos del proceso
    void* memoria_copia = malloc(memoria_usada);
    if (memoria_copia == NULL) {
        log_error(logger, "Error: No se pudo reservar memoria dinámica.\n");
        return NULL;
    }

    // Copiar los datos de la memoria de usuario al buffer local (usando uint32_t)
    memcpy(memoria_copia, (uint32_t*)memoria_usuario + (pcb->base / sizeof(uint32_t)), memoria_usada);

    log_info(logger, "Obtener memoria proceso exito");

    return memoria_copia;
}

// Función para buscar un proceso por su PID en la lista de procesos
t_mem_pcb* buscar_proceso_por_pid(uint32_t pid) {
    for (int i = 0; i < list_size(memory_process_list); i++) {

        t_mem_pcb* proceso = list_get(memory_process_list, i);
        
        if (proceso->PID == pid) {
            return proceso;  // Proceso encontrado
        }
    }
    log_error(logger, "Buscar proceso por pid retornó null");
    return NULL;  // Proceso no encontrado
}

void eliminar_proceso_mem(uint32_t pid) {
    bool buscar_por_pid(void* element) {
        t_mem_pcb* proceso = (t_mem_pcb*) element;
        return proceso->PID == pid;
    }

    // Buscar y eliminar el proceso de la lista global
    t_mem_pcb* proceso_a_eliminar = list_remove_by_condition(memory_process_list, buscar_por_pid);

    if (proceso_a_eliminar != NULL) {
        // Liberar los hilos asociados al proceso
        list_destroy_and_destroy_elements(proceso_a_eliminar->threads, free);
        free_mem_pcb(proceso_a_eliminar);

        // Loggear la eliminación exitosa del proceso
        // log_info(logger, "Proceso eliminado: PID=%d", pid);
        // Manejo según el tipo de partición
        if (partitioning_system != NULL && strcmp(partitioning_system, "DINAMICAS") == 0) {
            // Actualizar la lista global de particiones dinámicas
            update_dynamic_partition_list();
            log_info(logger, "Lista de particiones dinámicas actualizada tras eliminar el proceso PID=%d.", pid);
        } else if (partitioning_system != NULL && strcmp(partitioning_system, "FIJAS") == 0) {
            // Buscar la partición correspondiente y marcarla como libre
            bool buscar_por_pid_particion(void* element) {
                t_mem_partition* partition = (t_mem_partition*) element;
                return partition->PID == pid;
            }

            t_mem_partition* particion_a_liberar = list_find(partition_list, buscar_por_pid_particion);
            
            if (particion_a_liberar != NULL) {
                particion_a_liberar->isFree = 1;  // Marcar como libre
                particion_a_liberar->PID = -1;   // Resetear el PID
                log_info(logger, "Partición liberada: Base=%d, Límite=%d, Tamaño=%d",
                         particion_a_liberar->base, particion_a_liberar->limite, particion_a_liberar->size);
            } else {
                log_warning(logger, "No se encontró una partición asociada al proceso PID=%d en el esquema estático.", pid);
            }
        }
    } else {
        // Loggear si no se encontró el proceso
        log_warning(logger, "Proceso no encontrado para eliminación: PID=%d", pid);
    }
}

void eliminar_hilo_mem(uint32_t pid, uint32_t tid) {
    // Buscar el proceso por su PID
    t_mem_pcb* proceso = buscar_proceso_por_pid(pid);

    if (proceso == NULL) {
        log_error(logger, "No se encontró el proceso con PID=%d para eliminar el hilo TID=%d", pid, tid);
        return;
    }

    // Función auxiliar para buscar el hilo por su TID
    bool buscar_por_tid(void* element) {
        t_mem_tcb* hilo = (t_mem_tcb*) element;
        return hilo->TID == tid;
    }

    // Buscar y eliminar el hilo de la lista de hilos del proceso
    t_mem_tcb* hilo_a_eliminar = list_remove_by_condition(proceso->threads, buscar_por_tid);

    if (hilo_a_eliminar != NULL) {
        // Liberar recursos asociados al hilo
        free(hilo_a_eliminar->file_path);
        free(hilo_a_eliminar);

        if (partitioning_system != NULL && strcmp(partitioning_system, "DINAMICAS") == 0) {
            // Actualizar la lista global de particiones dinámicas
            update_dynamic_partition_list();
            // log_info(logger, "Lista de particiones dinámicas actualizada tras eliminar el proceso TID = %d.", tid);
        }

        log_info(logger, "## Hilo Destruido - (PID:TID) - (%i:%i)", pid, tid);
    } else {
        log_warning(logger, "No se encontró el hilo con TID = %d en el proceso PID = %d", tid, pid);
    }
}


// Función para agregar un hilo (TCB) a un proceso existente
t_mem_tcb* agregar_hilo_mem(t_mem_pcb* proceso, uint32_t tid, const char* file_path) {
    // Crear un nuevo hilo
    t_mem_tcb* new_thread = malloc(sizeof(t_mem_tcb));
    new_thread->TID = tid;
    init_registers(&(new_thread->regs)); // Inicializar los registros del hilo

    // Almacenar la ruta del archivo de pseudocódigo
    new_thread->file_path_len = strlen(file_path) + 1; // Incluye el carácter nulo '\0'
    new_thread->file_path = malloc(new_thread->file_path_len);
    strcpy(new_thread->file_path, file_path); // Copiar la ruta del archivo

    // Agregar el hilo a la lista de hilos del proceso
    list_add(proceso->threads, new_thread);

    // Loggear la adición del hilo
    // log_debug(logger, "Hilo agregado: TID=%d, Archivo=%s", tid, file_path);

    return new_thread;
}

//Funcion para calcular la memoria en uso
uint32_t calcular_memoria_en_uso() {
    uint32_t memoria_usada = 0;

    // Recorrer la lista global de procesos y sumar el tamaño de cada uno
    for (int i = 0; i < list_size(memory_process_list); i++) {
        t_mem_pcb* proceso = list_get(memory_process_list, i);
        memoria_usada += proceso->size;
    }

    return memoria_usada;
}

const char* verificar_espacio_y_crear_proceso(uint32_t pid, uint32_t size) {
    t_mem_partition* selected_partition = NULL;

    // Particiones estáticas
    if (partitioning_system != NULL && strcmp(partitioning_system, "FIJAS") == 0) {
        selected_partition = allocate_process_in_partition(partition_list, size, partitioning_criteria, pid);

        if (selected_partition != NULL) {
            uint32_t base = selected_partition->base; // Base de la partición seleccionada
            uint32_t limite = base + size;           // Límite calculado como base + tamaño del proceso
            crear_proceso_mem(pid, base, limite, size);

            log_info(logger, "Proceso %d (tamaño %d) asignado a partición fija (Base: %d, Límite: %d, Tamaño: %d)", 
                     pid, size, selected_partition->base, selected_partition->limite, selected_partition->size);

            return "Ok";

        } else {
            log_error(logger, "No hay partición adecuada para el proceso %d en particiones estáticas.", pid);
            return "Not Ok";
        }
    }

    // Particiones dinámicas
    else if (partitioning_system != NULL && strcmp(partitioning_system, "DINAMICAS") == 0) {
        // Actualizar la lista global de particiones dinámicas
        //update_dynamic_partition_list();

        selected_partition = allocate_process_in_partition(partition_list, size, partitioning_criteria, pid);

        if (selected_partition != NULL) {

            log_warning(logger, "The base of the selected partition is %d", selected_partition->base);

            uint32_t base = selected_partition->base; // Base de la partición seleccionada
            uint32_t limite = base + size;           // Límite calculado como base + tamaño del proceso

            crear_proceso_mem(pid, base, limite, size);

            update_dynamic_partition_list();

            log_info(logger, "Proceso %d asignado a partición dinámica (Base: %d, Límite: %d, Tamaño: %d)", 
                     pid, base, limite, size);
        } else {
            log_error(logger, "No hay suficiente espacio en memoria dinámica para el proceso %d.", pid);
            return "Not Ok";
        }
    }
    /*
    // Chequeo de memoria contigua estándar para control adicional
    uint32_t memoria_usada = calcular_memoria_en_uso();

    if (memoria_usada + size >= TAM_MEMORIA) {
        log_error(logger, "No hay suficiente espacio en memoria total para el proceso %d.", pid);
        return "Not Ok";
    }
    */
    return "Ok";
}

// Función para agregar un hilo a un proceso existente
const char* verificar_y_agregar_hilo(uint32_t pid, uint32_t tid, const char* file_path) {
    // Buscar el proceso con el PID dado
    t_mem_pcb* proceso = buscar_proceso_por_pid(pid);

    if (proceso == NULL) {
        // Si no existe el proceso, retornar error
        return "Not Ok"; 
    }

    // Si el proceso existe, agregar el nuevo hilo
    agregar_hilo_mem(proceso, tid, file_path);

    // Loggear la adición del hilo
    // log_info(logger, "Hilo TID=%d agregado al proceso PID=%d", tid, pid);

    return "Ok";  // Hilo agregado con éxito
}

void enviar_respuesta_ante_peticion(op_code operation_code, uint32_t fd) {
    op_code respuesta; //t_respuesta_syscall 

    respuesta = operation_code;

    ssize_t bytes_enviados = send(fd, &respuesta, sizeof(op_code), 0);
    
    if (bytes_enviados <= 0) {
        log_error(logger, "Error al enviar la respuesta de syscall");
        // Manejo de errores, según sea necesario  
    } else {
        log_warning(logger, "Respuesta de syscall enviada correctamente a %i", fd);
    }
}

t_register* obtener_registros_tcb(uint32_t pid, uint32_t tid) {
    // Iterar sobre la lista global de procesos en memoria
    for (int i = 0; i < list_size(memory_process_list); i++) {
        t_mem_pcb* proceso = list_get(memory_process_list, i);
        
        // Verificar si el PID coincide
        if (proceso->PID == pid) {
            // Buscar el hilo (TCB) en la lista de hilos del proceso
            for (int j = 0; j < list_size(proceso->threads); j++) {
                t_mem_tcb* hilo = list_get(proceso->threads, j);
                
                // Verificar si el TID coincide
                if (hilo->TID == tid) {
                    // log_info(logger, "Encontrado TID=%d en PID=%d", tid, pid);

                    t_register* set_registros = malloc(sizeof(t_register));

                    //Agregar registros del hilo
                    set_registros->AX = hilo->regs.AX;
                    set_registros->BX = hilo->regs.BX;
                    set_registros->CX = hilo->regs.CX;
                    set_registros->DX = hilo->regs.DX;
                    set_registros->EX = hilo->regs.EX;
                    set_registros->FX = hilo->regs.FX;
                    set_registros->GX = hilo->regs.GX;
                    set_registros->HX = hilo->regs.HX;
                    set_registros->PC = hilo->regs.PC;
                    //Agregar registros del proceso
                    set_registros->BASE = proceso->base;
                    set_registros->LIMITE = proceso->limite;

                    return set_registros;
                }
            }
            log_error(logger, "TID = %d no encontrado en el proceso PID = %d (obtener registros)", tid, pid);
            return NULL; // TID no encontrado en el proceso
        }
    }
    
    log_error(logger, "PID=%d no encontrado en la lista de procesos (obtener registros)", pid);
    return NULL; 
}

void send_instruction(char *mensaje, int socket_cliente) {
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

void send_execution_context(t_execution_context *execution_context, int socket_cliente) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->operation_code = EXECUTION_CONTEXT; 
    paquete->buffer = malloc(sizeof(t_buffer));

    serialize_execution_context(execution_context, paquete->buffer);

    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serialize_package(paquete, bytes);
    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    delete_package(paquete);
}

/* int send_all(int socket, void* buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = send(socket, buffer + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            perror("Error en send");
            return -1;
        }
        total_sent += sent;
    }
    return 0;
} */

void send_read_mem_data(uint32_t data_value, int socket_cliente) {
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->operation_code = READ_MEM; 

    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = sizeof(uint32_t); 
    paquete->buffer->stream = malloc(paquete->buffer->size);

    memcpy(paquete->buffer->stream, &data_value, sizeof(uint32_t));
    int bytes = sizeof(paquete->operation_code) + sizeof(paquete->buffer->size) + paquete->buffer->size;
    void *a_enviar = serialize_package(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    delete_package(paquete);
}

char* obtener_linea_pseudocodigo(u_int32_t pid, u_int32_t tid, u_int32_t pc) {
    
        t_mem_pcb* proceso = buscar_proceso_por_pid(pid);
        if (proceso == NULL)
        {
            log_error(logger, "No se encontró el proceso con PID: %i", pid);
            
            return NULL;
        }
        
        // Verificar si el PID coincide
        if (proceso->PID == pid) {
            // Buscar el hilo (TCB) en la lista de hilos del proceso
            for (int j = 0; j < list_size(proceso->threads); j++) {
                t_mem_tcb* hilo = list_get(proceso->threads, j);
                
                // Verificar si el TID coincide
                if (hilo->TID == tid) {
                    // log_info(logger, "Hilo encontrado: TID=%d en PID=%d", tid, pid);

                    const char *base_path = "./scripts/";
                    size_t total_length = strlen(base_path) + strlen(hilo->file_path) + 1;
                    char *full_path = malloc(total_length);

                    if (full_path == NULL) {
                        log_error(logger, "Memory allocation failed\n");
                        free(full_path);
                        return NULL;
                    }

                    strcpy(full_path, base_path);      // Copy base path
                    strcat(full_path, hilo->file_path); // Concatenate file path

                    // log_info(logger, "File path: %s", full_path);
                    // Abrir el archivo de pseudocódigo
                    FILE *file = fopen(full_path, "r");
                    if (file == NULL) {
                        log_error(logger, "No se pudo abrir el archivo: %s", hilo->file_path);
                        return NULL;
                    }

                    // Leer línea por línea hasta encontrar la línea correspondiente al PC
                    char *line = NULL;
                    size_t len = 0;
                    ssize_t read;
                    uint32_t current_line = 0;

                    while ((read = getline(&line, &len, file)) != -1) {
                        // Si el contador de línea es igual al PC, retornamos la línea
                        if (current_line == pc) {
                            fclose(file);
                            free(full_path);
                            return line;  // Importante: la memoria de 'line' debe liberarse luego
                        }
                        current_line++;
                    }

                    // Si no encontramos la línea, retornamos NULL
                    fclose(file);
                    free(full_path);
                    log_error(logger, "No se encontró la línea %d en el archivo: %s", pc, hilo->file_path);
                    return NULL;
                }
            }
    }
    
    log_error(logger, "PID=%d no encontrado en la lista de procesos (pseudocodigo)", pid);
    return NULL; 
}

void actualizar_registros(u_int32_t pid, u_int32_t tid, t_register* registros_actualizados) {
    // Iterar sobre la lista global de procesos en memoria
    for (int i = 0; i < list_size(memory_process_list); i++) {
        t_mem_pcb* proceso = list_get(memory_process_list, i);
        
        // Verificar si el PID coincide
        if (proceso->PID == pid) {
            // Actualizar los registros del proceso
            proceso->base = registros_actualizados->BASE;
            proceso->limite = registros_actualizados->LIMITE;
            
            // log_info(logger, "Registros del proceso PID=%d actualizados: BASE=0x%x, LIMITE=0x%x", 
            //         pid, proceso->base, proceso->limite);

            // Buscar el hilo (TCB) en la lista de hilos del proceso
            for (int j = 0; j < list_size(proceso->threads); j++) {
                t_mem_tcb* hilo = list_get(proceso->threads, j);

                // Verificar si el TID coincide
                if (hilo->TID == tid) {
                    // Actualizar los registros del hilo
                    hilo->regs.AX = registros_actualizados->AX;
                    hilo->regs.BX = registros_actualizados->BX;
                    hilo->regs.CX = registros_actualizados->CX;
                    hilo->regs.DX = registros_actualizados->DX;
                    hilo->regs.EX = registros_actualizados->EX;
                    hilo->regs.FX = registros_actualizados->FX;
                    hilo->regs.GX = registros_actualizados->GX;
                    hilo->regs.HX = registros_actualizados->HX;
                    hilo->regs.PC = registros_actualizados->PC;

                    // log_info(logger, "Registros del hilo TID=%d actualizados en el proceso PID=%d", tid, pid);
                    return;
                }
            }

            log_error(logger, "TID=%d no encontrado en el proceso PID=%d (actualizar registros)", tid, pid);
            return;
        }
    }

    log_error(logger, "PID=%d no encontrado en la lista de procesos (actualizar registros)", pid);
}


void log_memory_process_list() {
    if (memory_process_list == NULL || list_size(memory_process_list) == 0) {
        log_debug(logger, "La lista de procesos en memoria está vacía o no inicializada.");
        return;
    }

    log_debug(logger, "----- Estado actual de la lista de procesos en memoria -----");
    for (int i = 0; i < list_size(memory_process_list); i++) {
        t_mem_pcb* proceso = list_get(memory_process_list, i);
        log_debug(logger, "Proceso PID=%d | Base=0x%x | Límite=0x%x | Tamaño=%d bytes", 
                  proceso->PID, proceso->base, proceso->limite, proceso->size);

        if (list_size(proceso->threads) == 0) {
            log_debug(logger, "  No hay hilos asociados a este proceso.");
        } else {
            log_debug(logger, "  Hilos asociados al proceso PID=%d:", proceso->PID);
            for (int j = 0; j < list_size(proceso->threads); j++) {
                t_mem_tcb* hilo = list_get(proceso->threads, j);
                log_debug(logger, "    Hilo TID=%d | PC=%d | Ruta archivo=%s", 
                          hilo->TID, hilo->regs.PC, hilo->file_path);
            }
        }
    }
    log_debug(logger, "------------------------------------------------------------");
}

void free_mem_tcb(t_mem_tcb* tcb) {
    if (tcb == NULL) return;

    // Liberar el file_path si no es NULL
    if (tcb->file_path != NULL) {
        free(tcb->file_path);
        tcb->file_path = NULL; // Evitar accesos futuros a memoria ya liberada
    }

    // Liberar la estructura t_mem_tcb
    free(tcb);
}

void free_mem_pcb(t_mem_pcb* pcb) {
    if (pcb == NULL) return;

    // Liberar la estructura principal
    free(pcb);
}