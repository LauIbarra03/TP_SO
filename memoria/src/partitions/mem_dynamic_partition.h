#ifndef MEM_DYNAMIC_PARTITION_H
#define MEM_DYNAMIC_PARTITION_H

#include "../mem_structs.h"
#include <commons/collections/list.h>
#include <commons/log.h>

/**
 * @brief Genera una lista de particiones que representa el estado actual de la memoria.
 * 
 * @return t_list* Lista de `t_mem_partition` que representan las particiones de memoria actuales.
 */
t_list* build_dynamic_partition_list();

/**
 * @brief Actualiza la lista global de particiones dinámicas.
 */
void update_dynamic_partition_list();

/**
 * @brief Registra el estado actual de las particiones de memoria en el log.
 */
void log_estado_particiones();

#endif /* MEM_DYNAMIC_PARTITION_H */
