#include "mem_dynamic_partition.h"
#include <commons/collections/list.h>
#include <stdlib.h>

extern t_list *memory_process_list;
extern t_list *partition_list;  // Variable global para particiones dinámicas
extern uint32_t TAM_MEMORIA;
extern t_log *logger;

bool ordenar_por_base(void* a, void* b) {
    t_mem_pcb* pcb_a = (t_mem_pcb*)a;
    t_mem_pcb* pcb_b = (t_mem_pcb*)b;
    return pcb_a->base < pcb_b->base;
}

t_list* build_dynamic_partition_list() {
    t_list *partitions = list_create();

    if (list_is_empty(memory_process_list)) {
        t_mem_partition *single_partition = malloc(sizeof(t_mem_partition));
        single_partition->PID = -1;
        single_partition->base = 0;
        single_partition->limite = TAM_MEMORIA;
        single_partition->size = TAM_MEMORIA;
        single_partition->isFree = 1;
        list_add(partitions, single_partition);
    } else {
        // Ordenar la lista por base antes de procesar
        list_sort(memory_process_list, ordenar_por_base);

        uint32_t current_base = 0;

        for (int i = 0; i < list_size(memory_process_list); i++) {
            t_mem_pcb *pcb = list_get(memory_process_list, i);

            if (current_base < pcb->base) {
                t_mem_partition *free_partition = malloc(sizeof(t_mem_partition));
                free_partition->PID = -1;
                free_partition->base = current_base;
                free_partition->limite = pcb->base;
                free_partition->size = pcb->base - current_base;
                free_partition->isFree = 1;
                list_add(partitions, free_partition);
            }

            t_mem_partition *occupied_partition = malloc(sizeof(t_mem_partition));
            occupied_partition->PID = pcb->PID;
            occupied_partition->base = pcb->base;
            occupied_partition->limite = pcb->limite;
            occupied_partition->size = pcb->limite - pcb->base;
            occupied_partition->isFree = 0;
            list_add(partitions, occupied_partition);

            current_base = pcb->limite;
        }

        if (current_base < TAM_MEMORIA) {
            t_mem_partition *free_partition_end = malloc(sizeof(t_mem_partition));
            free_partition_end->PID = -1;
            free_partition_end->base = current_base;
            free_partition_end->limite = TAM_MEMORIA;
            free_partition_end->size = TAM_MEMORIA - current_base;
            free_partition_end->isFree = 1;
            list_add(partitions, free_partition_end);
        }
    }

    return partitions;
}

void update_dynamic_partition_list() {
    // Liberar la lista anterior de particiones si existe
    if (partition_list != NULL) {
        list_destroy_and_destroy_elements(partition_list, free);
    }

    // Actualizar la lista global con el estado actual
    partition_list = build_dynamic_partition_list();
}

void log_estado_particiones() {
    if (partition_list == NULL) {
        log_warning(logger, "No hay particiones disponibles para mostrar.");
        return;
    }

    log_info(logger, "Estado actual de las particiones de memoria:");

    for (int i = 0; i < list_size(partition_list); i++) {
        t_mem_partition* partition = list_get(partition_list, i);

        if (partition->isFree) {
            log_info(logger, "Partición %d - Libre, Base: %d, Límite: %d, Tamaño: %d",
                     i, partition->base, partition->limite, partition->size);
        } else {
            log_info(logger, "Partición %d - Ocupada, PID: %d, Base: %d, Límite: %d, Tamaño: %d",
                     i, partition->PID, partition->base, partition->limite, partition->size);
        }
    }
}
