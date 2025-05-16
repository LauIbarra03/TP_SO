// partitions.c

#include "mem_structs.h"
#include "partitions.h"
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/log.h>

extern char* partitioning_system; 

// Función para construir la lista de particiones a partir de un arreglo de tamaños
t_list* build_static_partition_list(char **partitions) {
    t_list *partition_list = list_create();

    for (int i = 0; partitions[i] != NULL; i++) {
        int size = atoi(partitions[i]); // Convertir el tamaño de string a int

        t_mem_partition *new_partition = malloc(sizeof(t_mem_partition));
        new_partition->size = size;
        new_partition->isFree = 1;  // Inicia como libre
        new_partition->PID = -1;   // PID -1 indica que está libre
        new_partition->base = (i == 0) ? 0 : ((t_mem_partition *)list_get(partition_list, i - 1))->limite;
        new_partition->limite = new_partition->base + size;

        list_add(partition_list, new_partition);
    }
    return partition_list;
}

t_mem_partition* allocate_process_in_partition(t_list *partition_list, uint32_t process_size, const char *criteria, int pid) {
    t_mem_partition *selected_partition = NULL;
    int partition_index = -1; // Índice de la partición asignada
    // TODO, consultar que es esto
    // log_warning(logger, "Size: %d");

    if (strcmp(criteria, "FIRST") == 0) {

        void *find_first_fit(void *element) {
            t_mem_partition *partition = (t_mem_partition *)element;
            // log_warning(logger, "\n\nisFree %d; size %d; process_size %d\n\n", partition->isFree, partition->size, process_size);
            // TODO, consultar warning
            return partition->isFree && partition->size >= process_size;
        }

        // log_warning(logger, "ME EJECUTÉ");
        selected_partition = list_find(partition_list, find_first_fit);

    } else if (strcmp(criteria, "BEST") == 0) {
        int min_size = __INT_MAX__;
        for (int i = 0; i < list_size(partition_list); i++) {
            t_mem_partition *partition = list_get(partition_list, i);
            if (partition->isFree && partition->size >= process_size && partition->size < min_size) {
                min_size = partition->size;
                selected_partition = partition;
                partition_index = i; // Actualizar el índice de la mejor partición
            }
        }
    } else if (strcmp(criteria, "WORST") == 0) {
        int max_size = 0;
        for (int i = 0; i < list_size(partition_list); i++) {
            t_mem_partition *partition = list_get(partition_list, i);
            if (partition->isFree && partition->size >= process_size && partition->size > max_size) {
                max_size = partition->size;
                selected_partition = partition;
                partition_index = i; // Actualizar el índice de la peor partición
            }
        }
    }

    if (selected_partition != NULL) {
        selected_partition->isFree = 0;  // Marcar la partición como ocupada
        selected_partition->PID = pid;   // Asignar el PID del proceso

        // Si no se actualizó el índice en BEST/WORST, buscar el índice en FIRST
        if (partition_index == -1) {
            for (int i = 0; i < list_size(partition_list); i++) {
                if (list_get(partition_list, i) == selected_partition) {
                    partition_index = i;
                    break;
                }
            }
        }

        /*
        log_info(logger, "Proceso %d asignado (Tamaño: %d) a partición #%d (Base: %d, Tamaño: %d)",
                 pid, process_size, partition_index, selected_partition->base, selected_partition->size);
                 */

        return selected_partition;

    } else {
        log_error(logger, "No se pudo encontrar una partición adecuada para el proceso %d.", pid);
    }

    return selected_partition;  // Retornar la partición seleccionada o NULL
}

// Función para imprimir el estado actual de las particiones
void print_partitions(t_list *partition_list) {
    log_info(logger, "Partition List State:");

    for (int i = 0; i < list_size(partition_list); i++) {
        t_mem_partition *partition = list_get(partition_list, i);
        log_info(logger, "Partition %d - Base: %d, Limit: %d, Size: %d, Status: %s, PID: %d",
                 i, partition->base, partition->limite, partition->size,
                 partition->isFree ? "Free" : "Occupied", partition->PID);
    }
}

void print_partitions_summary(t_list *partition_list) {
    char summary[1024] = "Partition Summary: ";
    char partition_info[64];

    for (int i = 0; i < list_size(partition_list); i++) {
        t_mem_partition *partition = list_get(partition_list, i);
        if (partition->isFree) {
            snprintf(partition_info, sizeof(partition_info), "%d ( )", partition->size);
        } else {
            snprintf(partition_info, sizeof(partition_info), "%d (%d)", partition->size, partition->PID);
        }

        strcat(summary, partition_info);
        if (i < list_size(partition_list) - 1) {
            strcat(summary, ", ");
        }
    }

    log_debug(logger, "%s", summary);
}


// Función para liberar la memoria utilizada por la lista de particiones
void free_partition_list(t_list *partition_list) {
    list_destroy_and_destroy_elements(partition_list, free);
}
