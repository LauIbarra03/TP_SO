#ifndef THREAD_OPERATIONS_H
#define THREAD_OPERATIONS_H

#include <tcb_lists.h>
#include <pcb_lists.h>
#include <kernel.h>

typedef struct {
    t_tcb* tcb;
    uint32_t tid_blockeador;
    struct t_thread_join* proximo_elemento;
} t_thread_join; 

extern t_thread_join* lista_tcbs_blockeados;

t_tcb* crear_thread(t_pcb* pcb, char* file, uint32_t priority);
void block_tcb_until_finish(t_tcb* tcb, uint32_t tid);
void agregar_tcb_blockeado(t_thread_join** lista, t_tcb* tcb_bloqueado, uint32_t tid_bloqueador);
void desbloquear_thread_por_join(uint32_t tid_finished);
void unlock_tcb(t_tcb* tcb);



#endif