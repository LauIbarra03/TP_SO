#include "pcb_lists.h"
#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <utils/pcb_serialization.h>

extern t_log *logger;

// Declaración de las listas de PCBs
t_list *list_PCB_active = NULL;
t_list *list_PCB_new = NULL;
t_list *list_PCB_exit = NULL;

// Declaración de los mutex de las listas
pthread_mutex_t mutex_list_PCB_active = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_list_PCB_exit = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_list_PCB_new = PTHREAD_MUTEX_INITIALIZER;

t_pcb* current_pcb_executing = NULL;

// Función para inicializar las listas y los mutex
void init_pcb_lists() {
    list_PCB_active = list_create();
    list_PCB_exit = list_create();
    list_PCB_new = list_create();

    if (pthread_mutex_init(&mutex_list_PCB_active, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex para list_PCB_active");
    } else {
        log_info(logger, "Mutex para list_PCB_active inicializado correctamente");
    }

    if (pthread_mutex_init(&mutex_list_PCB_exit, NULL) != 0) {
        log_error(logger, "Error al inicializar el mutex para list_PCB_exit");
    } else {
        log_info(logger, "Mutex para list_PCB_exit inicializado correctamente");
    }
}

// Función para destruir las listas y los mutex
void destroy_pcb_lists() {
    list_destroy_and_destroy_elements(list_PCB_active, (void *)delete_pcb);
    list_destroy_and_destroy_elements(list_PCB_exit, (void *)delete_pcb);

    pthread_mutex_destroy(&mutex_list_PCB_active);
    pthread_mutex_destroy(&mutex_list_PCB_exit);

    log_info(logger, "Listas y mutexes de PCBs destruidos correctamente");
}

void add_pcb_to_active(t_pcb *pcb) {
    pthread_mutex_lock(&mutex_list_PCB_active);
    list_add(list_PCB_active, pcb);
    pthread_mutex_unlock(&mutex_list_PCB_active);

    log_info(logger, "PCB con ID %d añadido a list_PCB_active", pcb->PID);
}

void add_pcb_to_new(t_pcb *pcb) {
    pthread_mutex_lock(&mutex_list_PCB_new);
    list_add(list_PCB_new, pcb);
    pthread_mutex_unlock(&mutex_list_PCB_new);

    log_info(logger, "PCB con ID %d añadido a list_PCB_new", pcb->PID);
}

// Función para mover un PCB de activos a salida
void move_pcb_to_exit(uint32_t pid) {
    pthread_mutex_lock(&mutex_list_PCB_active);

    bool pid_condition(void *pcb) {
        return ((t_pcb *)pcb)->PID == pid;
    }

    t_pcb *pcb = list_remove_by_condition(list_PCB_active, pid_condition);
    pthread_mutex_unlock(&mutex_list_PCB_active);

    if (pcb != NULL) {
        pcb->state = EXIT;

        pthread_mutex_lock(&mutex_list_PCB_exit);
        list_add(list_PCB_exit, pcb);
        pthread_mutex_unlock(&mutex_list_PCB_exit);

        log_info(logger, "PCB con ID %d movido a list_PCB_exit", pcb->PID);
    } else {
        log_warning(logger, "No se encontró un PCB con ID %d en list_PCB_active para mandar a exit", pid);
    }
}

// Función para buscar un PCB en la lista de activos por su PID
t_pcb *find_pcb_by_pid(uint32_t pid) {

    pthread_mutex_lock(&mutex_list_PCB_active);

    bool pid_condition(void *pcb) {


        return ((t_pcb *)pcb)->PID == pid;
    }

    t_pcb *pcb = list_find(list_PCB_active, pid_condition);

    pthread_mutex_unlock(&mutex_list_PCB_active);

    return pcb;
}

t_pcb *find_pcb_by_pid_NEW(uint32_t pid) {

    pthread_mutex_lock(&mutex_list_PCB_new);

    bool pid_condition(void *pcb) {


        return ((t_pcb *)pcb)->PID == pid;
    }

    t_pcb *pcb = list_find(list_PCB_active, pid_condition);

    pthread_mutex_unlock(&mutex_list_PCB_new);

    return pcb;
}

// Función para registrar el contenido de una lista en el log
void log_list_contents_pcb(t_list *list, const char *list_name) {
    log_info(logger, "Contenido de la lista %s:", list_name);

    pthread_mutex_lock(list == list_PCB_active ? &mutex_list_PCB_active : &mutex_list_PCB_exit);
    for (int i = 0; i < list_size(list); i++) {
        t_pcb *pcb = list_get(list, i);
        log_info(logger, "PCB ID: %d, Estado: %d", pcb->PID, pcb->state);
    }
    pthread_mutex_unlock(list == list_PCB_active ? &mutex_list_PCB_active : &mutex_list_PCB_exit);

    log_info(logger, "Total de PCBs en la lista %s: %d", list_name, list_size(list));
}

// Función para contar el número de PCBs en una lista
int count_pcbs_in_list(t_list *list) {
    pthread_mutex_lock(list == list_PCB_active ? &mutex_list_PCB_active : &mutex_list_PCB_exit);
    int count = list_size(list);
    pthread_mutex_unlock(list == list_PCB_active ? &mutex_list_PCB_active : &mutex_list_PCB_exit);
    return count;
}

// Función para eliminar un PCB de la lista activa por su PID
t_pcb *remove_pcb_from_active(uint32_t pid) {
    pthread_mutex_lock(&mutex_list_PCB_active);

    bool pid_condition(void *pcb) {
        return ((t_pcb *)pcb)->PID == pid;
    }

    t_pcb *pcb = list_remove_by_condition(list_PCB_active, pid_condition);
    pthread_mutex_unlock(&mutex_list_PCB_active);

    if (pcb) {
        log_info(logger, "PCB con ID %d eliminado de list_PCB_active", pcb->PID);
    } else {
        log_warning(logger, "No se encontró un PCB con ID %d en list_PCB_active para eliminar", pid);
    }

    return pcb;
}
