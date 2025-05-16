#include <long_term_list.h>
#include <pcb_lists.h>
#include <utils/communication.h>
#include <process.h>
#include <utils/serializators/syscalls_serialization/process_create_serialization/process_create_serialization.h>

//add_pcb_to_active(pcb);

extern t_list *list_NEW_TCB;
extern t_list *list_READY_TCB;

extern t_list *list_PCB_new;

extern t_list *list_first_thread_params;

extern sem_t sem_thread_count;

extern pthread_mutex_t mutex_list_NEW_TCB;
extern pthread_mutex_t mutex_list_READY_TCB;

extern t_log *logger;  // Usar el logger externamente

void init_long_term_scheduler() {
    list_first_thread_params = list_create();
    log_info(logger, "Inicializada la lista del long term scheduler");
}

bool try_create_NEW_process() {

    int tamanio_queue = list_size(list_PCB_new);

    if (tamanio_queue == 0) {
        log_error(logger, "La lista new esta vacia");
        return false;
    }

    t_pcb *primer_pcb = list_get(list_PCB_new, 0);

    if (primer_pcb == NULL) {
        log_error(logger, "El primer pcb encontrado en new es NULL");
        return false;
    }

    op_code mem_response_pcb = create_process_memory(primer_pcb);

    t_process_create *peticion_create = list_get(list_first_thread_params, 0);

    if (peticion_create == NULL) {
        log_error(logger, "La peticion create es NULL");
        return false;
    }

    if (mem_response_pcb == OK)
    {
        t_tcb *tcb = create_thread(primer_pcb, peticion_create->nombre_archivo, peticion_create->prioridad);

        list_remove(list_first_thread_params, 0);
        list_remove(list_PCB_new, 0);

        destroy_process_create(peticion_create);

        log_info(logger, "## (PID:TID) : (%i:0) Se crea el proceso - Estado: NEW", tcb->parent_PID);

        if (tcb->state == READY)
        {
            add_pcb_to_active(primer_pcb);
        } 

        log_debug(logger, "Se pudo crear el hilo y proceso del long term.");

        return true;
    }
    else
    {
        // En caso de error, muevo el tcb de NEW a EXIT
        log_error(logger, "No se pudo crear el hilo en memoria. Dejando el tcb en NEW.");

        return false;
    }

}

void try_create_NEW_process_loop() {

    bool continuar = true;

    while (continuar) {
        continuar = try_create_NEW_process();
    }
}