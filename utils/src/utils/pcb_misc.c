//pcb_misc.c
#include <stdlib.h>
#include <string.h>
#include "pcb_misc.h"
#include <utils/tcb_serialization.h>
#include <utils/pcb_serialization.h>
#include <commons/log.h>
#include <utils/estructuras.h>

// Aseguramos que logger esté disponible externamente
extern t_log* logger;

// Función para agregar un tcb a un pcb
void add_tcb_to_pcb(t_pcb* pcb, t_tcb* tcb) {
    // Actualizar el parent_PID del tcb con el PID del pcb
    tcb->parent_PID = pcb->PID;

    // 1. Añadir el TID al array de TIDs
    uint32_t* new_tid_array = realloc(pcb->TID, (pcb->tid_count + 1) * sizeof(uint32_t));
    if (new_tid_array == NULL) {
        log_error(logger, "Error al reallocar memoria para el array de TIDs");
        exit(EXIT_FAILURE);
    }
    pcb->TID = new_tid_array;
    pcb->TID[pcb->tid_count] = tcb->TID;
    pcb->tid_count++;

    // 2. Añadir el TCB al array de TCBs
    log_debug(logger, "Se sumo uno a la cuenta de num tcbs");
    t_tcb* new_tcb_array = realloc(pcb->tcbs, (pcb->num_tcbs + 1) * sizeof(t_tcb));
    if (new_tcb_array == NULL) {
        log_error(logger, "Error al reallocar memoria para el array de TCBs");
        exit(EXIT_FAILURE);
    }
    pcb->tcbs = new_tcb_array;
    memcpy(&(pcb->tcbs[pcb->num_tcbs]), tcb, sizeof(t_tcb));
    pcb->num_tcbs++;
}

// Función para eliminar un tcb de un pcb dado su TID
void remove_tcb_from_pcb(t_pcb* pcb, uint32_t tid) {
    int index = -1;

    // 1. Buscar el TID en el array de TIDs
    for (uint32_t i = 0; i < pcb->tid_count; i++) {
        if (pcb->TID[i] == tid) {
            index = i;
            break;
        }
    }

    // Si el TID no se encuentra, salir de la función
    if (index == -1) {
        log_error(logger, "TID no encontrado en el PCB");
        return;
    }

    // 2. Eliminar el TID del array de TIDs
    for (uint32_t i = index; i < pcb->tid_count - 1; i++) {
        pcb->TID[i] = pcb->TID[i + 1];
    }
    pcb->tid_count--;
    if (pcb->tid_count > 0) {
        uint32_t* new_tid_array = realloc(pcb->TID, pcb->tid_count * sizeof(uint32_t));
        if (new_tid_array == NULL) {
            log_error(logger, "Error al reallocar memoria para el array de TIDs");
            exit(EXIT_FAILURE);
        }
        pcb->TID = new_tid_array;
    } else {
        free(pcb->TID);
        pcb->TID = NULL;
    }

    // 3. Eliminar el TCB correspondiente del array de TCBs
    destroy_tcb(&(pcb->tcbs[index]));
    for (uint32_t i = index; i < pcb->num_tcbs - 1; i++) {
        memcpy(&(pcb->tcbs[i]), &(pcb->tcbs[i + 1]), sizeof(t_tcb));
    }
    pcb->num_tcbs--;
    if (pcb->num_tcbs > 0) {
        t_tcb* new_tcb_array = realloc(pcb->tcbs, pcb->num_tcbs * sizeof(t_tcb));
        if (new_tcb_array == NULL) {
            log_error(logger, "Error al reallocar memoria para el array de TCBs");
            exit(EXIT_FAILURE);
        }
        pcb->tcbs = new_tcb_array;
    } else {
        free(pcb->tcbs);
        pcb->tcbs = NULL;
    }
}
