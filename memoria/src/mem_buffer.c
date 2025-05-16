#include "mem_buffer.h"
#include <utils/communication.h>
#include <stdlib.h>
#include <string.h>

void inicializar_memoria() {
    memoria_usuario = malloc(TAM_MEMORIA);
    if (memoria_usuario == NULL) {
        log_error(logger, "Error al asignar memoria de usuario");
        exit(EXIT_FAILURE);
    }

    memset(memoria_usuario, 0, TAM_MEMORIA);
    
    log_info(logger, "Memoria de usuario inicializada con tamaño: %d", TAM_MEMORIA);
}

void escribir_en_memoria(uint32_t valor, void* buffer, uint32_t posicion) {
    memcpy(buffer + posicion, &valor, sizeof(uint32_t));
}

uint32_t leer_uint32_t_en_buffer(uint32_t posicion, void* buffer) {
    uint32_t valor_leido;

    memcpy(&valor_leido, buffer + posicion, sizeof(uint32_t));
    return valor_leido;
}

void destruir_buffer() {
    if(memoria_usuario != NULL) {
        free(memoria_usuario);
        memoria_usuario = NULL;
    }
}

// Testing
void test_escribir_leer_memoria() {
    inicializar_memoria();

    uint32_t valor_a_escribir = 42;
    uint32_t posicion = 100;

    log_info(logger, "Escribiendo valor %u en la posición %u", valor_a_escribir, posicion);
    escribir_en_memoria(valor_a_escribir, memoria_usuario, posicion);
    uint32_t valor_leido = leer_uint32_t_en_buffer(posicion, memoria_usuario);
    log_info(logger, "Valor leído desde la posición %u: %u", posicion, valor_leido);
    destruir_buffer(memoria_usuario);
}