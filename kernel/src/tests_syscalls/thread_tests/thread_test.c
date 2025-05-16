#include "thread_test.h"
#include <kernel.h>

/*

void thread_create_test() {
    t_pcb* new_pcb = create_process(10, "Hola.c", 1);
    t_tcb* new_tcb = create_thread(new_pcb,"Hilo1.c", 1);

    log_info(logger, "Iniciando test creacion de hilo");
    // Testeo que al crear el thread este este en cola de READY
    t_tcb* tcb_de_lista = list_pop_first_tcb(list_READY_TCB);

    log_info(logger, "PID proceso: %d\n", new_pcb->PID);
    log_info(logger, "PPID nuevo hilo: %d\n", new_tcb->parent_PID);
    log_info(logger, "Tid hilo creado: %d\n", new_tcb->TID);
    log_info(logger, "TID cola ready %d\n", tcb_de_lista->TID);
    
}

void thread_join_test() {
    t_pcb* new_pcb = create_process(10, "Hola.c", 1);
    t_tcb* new_tcb = create_thread(new_pcb,"Hilo1.c", 1);

    log_info(logger, "Iniciando test de bloqueado de TCB, el estado del tcb es: %d\n", new_tcb->state);

    block_tcb_until_finish(new_tcb, 2);

    log_info(logger, "El estado del tcb ahora es: %d deberia ser '3'\n", new_tcb->state);
    
    t_tcb* tcb_de_lista = find_tcb_in_list(list_BLOCKED_TCB, new_tcb->parent_PID, new_tcb->TID);
    if(new_tcb->TID == tcb_de_lista->TID) {
        log_info(logger, "El tcb se bloqueo correctamente y esta en lista de blocked");
    } else {
        log_info(logger, "No se bloqueo correctamente el tcb");
    }
}

void thread_cancel_test() {
    t_pcb* new_pcb = create_process(10, "Hola.c", 1);
    t_tcb* new_tcb = create_thread(new_pcb,"Hilo1.c", 1);

    log_info(logger, "Iniciando test de thread_cancel");

    block_tcb_until_finish(new_tcb, 2);

    log_info(logger, "El estado del tcb antes de desbloquearse es: %d deberia ser: %d", new_tcb->state, BLOCKED);

    desbloquear_thread_por_join(2);
    if(new_tcb->state == READY) {
        log_info(logger, "Se desbloqueo correctamente el tcb");
    }
}

void thread_exit_test(){
    t_pcb* new_pcb = create_process(10, "Hola.c", 1);
    t_tcb* new_tcb = create_thread(new_pcb,"Hilo1.c", 1);

    log_info(logger, "Iniciando test de thread exit, verificando si el proceso se agrega correctamente a exit");
    log_info(logger, "El estado del tcb antes de exit es: %d", new_tcb->state);

    add_tcb_to_list(list_EXIT_TCB, new_tcb);
    new_tcb->state = EXIT;

    log_info(logger, "El estado del tcb luego de agregarse a la lista es: %d, deberia ser: %d", new_tcb->state, EXIT);   

}




*/