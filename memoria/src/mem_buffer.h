#ifndef MEM_BUFFER_H
#define MEM_BUFFER_H

#include "mem_structs.h"
#include <stdint.h> // Para usar uint32_t
#include <commons/log.h> // Para usar t_log
#include <commons/collections/list.h> // Para usar t_list
#include <utils/communication.h>
#include <utils/estructuras.h>

// 
extern uint32_t TAM_MEMORIA;
extern void* memoria_usuario;

//Declaracion de la funcion para inicializar el valor de la memoria
void inicializar_memoria(); 

/**
 * @brief Escribe un valor uint32_t en una posicon fija del buffer
 * 
 * @param valor el valor uint32_t que quieres escribir en la memoria
 * @param buffer el puntero al inicio del buffer de memoria donde se almacenan los datos
 * @param posicion el indice del buffer donde queres escribir el valor
 */
void escribir_en_memoria(uint32_t valor, void* buffer, uint32_t posicion);

/**
 * @brief Lee un valor uint32_t desde una posicion fija del buffer
 * 
 * @param posicion el indice del buffer donde queres leer el valor
 * @param buffer el puntero al inicio del buffer de memoria donde se almancenan los datos
 */
uint32_t leer_uint32_t_en_buffer(uint32_t posicion, void* buffer);

//Funcion para destruir el buffer y liberar memoria
void destruir_buffer();

// Testing
void test_escribir_leer_memoria();

#endif /* MEM_BUFFER_H */