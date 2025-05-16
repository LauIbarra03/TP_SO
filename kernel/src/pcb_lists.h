#ifndef PCB_LISTS_H_
#define PCB_LISTS_H_

#include <commons/collections/list.h>
#include <utils/estructuras.h>
#include <pthread.h>

// Declaración de listas para PCBs
extern t_list *list_PCB_active;
extern t_list *list_PCB_exit;

// Declaración de mutexes para proteger las listas
extern pthread_mutex_t mutex_list_PCB_active;
extern pthread_mutex_t mutex_list_PCB_exit;

extern t_pcb* current_pcb_executing;

// Funciones para inicializar y destruir las listas de PCBs
/**
 * @brief Inicializa las listas de PCBs y sus mutexes.
 */
void init_pcb_lists();

/**
 * @brief Destruye las listas de PCBs y sus mutexes.
 */
void destroy_pcb_lists();

// Funciones para manejar PCBs en listas
/**
 * @brief Agrega un PCB a la lista de PCBs activos.
 * 
 * @param pcb Puntero al PCB que se va a añadir.
 */
void add_pcb_to_active(t_pcb *pcb);

void add_pcb_to_new(t_pcb *pcb);

/**
 * @brief Mueve un PCB de la lista de activos a la lista de salida (exit).
 * 
 * @param pid Identificador del PCB que se va a mover.
 */
void move_pcb_to_exit(uint32_t pid);

/**
 * @brief Busca un PCB en la lista de activos por su PID.
 * 
 * @param pid Identificador del PCB que se busca.
 * @return t_pcb* Puntero al PCB encontrado, o NULL si no se encuentra.
 */
t_pcb *find_pcb_by_pid(uint32_t pid);

t_pcb *find_pcb_by_pid_NEW(uint32_t pid);

/**
 * @brief Muestra el contenido de una lista de PCBs en el log.
 * 
 * @param list Puntero a la lista de PCBs que se quiere mostrar.
 * @param list_name Nombre de la lista para el log.
 */
void log_list_contents_pcb(t_list *list, const char *list_name);

/**
 * @brief Cuenta el número de PCBs en una lista específica.
 * 
 * @param list Lista de PCBs cuyo tamaño se desea obtener.
 * @return int Número de elementos en la lista.
 */
int count_pcbs_in_list(t_list *list);

/**
 * @brief Elimina un PCB de la lista de activos por su PID.
 * 
 * @param pid Identificador del PCB que se desea eliminar.
 * @return t_pcb* Puntero al PCB eliminado, o NULL si no se encuentra.
 */
t_pcb *remove_pcb_from_active(uint32_t pid);

#endif /* PCB_LISTS_H_ */
