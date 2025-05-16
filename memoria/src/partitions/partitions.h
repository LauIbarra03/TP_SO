#ifndef MEM_PARTITIONS_H
#define MEM_PARTITIONS_H

#include <stdint.h>          // Para usar uint32_t
#include <commons/log.h>     // Para usar t_log
#include <commons/collections/list.h> // Para usar t_list
#include "../mem_structs.h"     // Para usar t_mem_partition
#include "mem_dynamic_partition.h"

// Declaración de funciones

/**
 * @brief Construye la lista inicial de particiones a partir de un arreglo de tamaños.
 * 
 * @param partitions Arreglo de cadenas que contienen los tamaños de las particiones.
 * @return t_list* Lista de particiones inicializada.
 */
t_list* build_static_partition_list(char **partitions);

/**
 * @brief Asigna un proceso a una partición según el criterio especificado.
 * 
 * @param partition_list Lista de particiones donde se realizará la asignación.
 * @param process_size Tamaño del proceso que se desea asignar.
 * @param criteria Criterio de asignación: "FIRST", "BEST" o "WORST".
 * @param pid Identificador único del proceso.
 * @return t_mem_partition* Puntero a la partición asignada, o NULL si no se encontró una adecuada.
 */
t_mem_partition* allocate_process_in_partition(t_list *partition_list, uint32_t process_size, const char *criteria, int pid);


/**
 * @brief Imprime el estado actual de las particiones.
 * 
 * @param partition_list Lista de particiones.
 */
void print_partitions(t_list *partition_list);

/**
 * @brief Imprime el estado actual de las particiones de forma resumida en una línea.
 * 
 * @param partition_list Lista de particiones.
 */
void print_partitions_summary(t_list *partition_list);

/**
 * @brief Libera la memoria utilizada por la lista de particiones.
 * 
 * @param partition_list Lista de particiones.
 */
void free_partition_list(t_list *partition_list);

#endif /* MEM_PARTITIONS_H */
