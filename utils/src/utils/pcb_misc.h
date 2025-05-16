#ifndef PCB_MISC_H
#define PCB_MISC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // Para tipos uint32_t
#include <utils/pcb_serialization.h>
#include <utils/tcb_serialization.h>

void add_tcb_to_pcb(t_pcb* pcb, t_tcb* tcb);

void remove_tcb_from_pcb(t_pcb* pcb, uint32_t tid);

#endif