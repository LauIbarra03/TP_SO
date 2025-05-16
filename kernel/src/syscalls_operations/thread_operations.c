#include "thread_operations.h"


t_thread_join* lista_tcbs_blockeados = NULL;

void block_tcb_until_finish(t_tcb* tcb, uint32_t tid) {

    // tcb es el tcb actual que se debe bloquear
    // tid es el tid del hilo que debe finalizar para desbloquearse

    if((tcb->state) == EXIT /*|| existing_tcb(tcb->parent_PID, tid) != 1*/) {
        //log_info(logger, "El hilo a hacer join no existe o ya esta finalizo" );
        return;
    } else {

        block_tcb(tcb); 
        agregar_tcb_blockeado(&lista_tcbs_blockeados, tcb, tid);

    }

    current_tcb_executing = find_tcb_from_tid(tid, current_pcb_executing->PID);
}

// Función para agregar un nodo al final de la lista
void agregar_tcb_blockeado(t_thread_join** lista, t_tcb* tcb_bloqueado, uint32_t tid_bloqueador) {
    // Crear un nuevo nodo
    t_thread_join* nuevo_nodo = (t_thread_join*)malloc(sizeof(t_thread_join));
    if (nuevo_nodo == NULL) {
        // Manejo de error en caso de que no se pueda asignar memoria
        perror("Error al asignar memoria para el nuevo nodo");
        exit(EXIT_FAILURE);
    }

    // Inicializar el nuevo nodo
    nuevo_nodo->tcb = tcb_bloqueado;
    nuevo_nodo->tid_blockeador = tid_bloqueador;
    nuevo_nodo->proximo_elemento= NULL;

    // Si la lista está vacía, el nuevo nodo es el primer elemento
    if (*lista == NULL) {
        *lista = nuevo_nodo;
    } else {
        // Si no está vacía, recorrer hasta el último nodo
        t_thread_join* actual = *lista;
        while (actual->proximo_elemento != NULL) {
            actual = actual->proximo_elemento;
        }
        // Enlazar el último nodo al nuevo nodo
        actual->proximo_elemento = nuevo_nodo;
    }
}

// Función para desbloquear el tcb bloqueado por `join`
void desbloquear_thread_por_join(uint32_t tid_finished) {
    t_tcb* tcb_a_desbloquear = NULL;
    t_thread_join* actual = lista_tcbs_blockeados;
    t_thread_join* anterior = NULL;

    while (actual != NULL) {
        if (actual->tid_blockeador == tid_finished) {
            tcb_a_desbloquear = actual->tcb;

            // Eliminar el nodo actual de la lista
            if (anterior == NULL) {
                lista_tcbs_blockeados = actual->proximo_elemento;
            } else {
                anterior->proximo_elemento = actual->proximo_elemento;
            }

            free(actual);  // Liberar memoria del nodo eliminado
            break;
        }
        anterior = actual;
        actual = actual->proximo_elemento;
    }

    if (tcb_a_desbloquear != NULL) {
        unlock_tcb(tcb_a_desbloquear);
    }


}

void unlock_tcb(t_tcb* tcb) {
        pthread_mutex_lock(&mutex_list_BLOCKED_TCB);
        pthread_mutex_lock(&mutex_list_READY_TCB);
        move_tcb_between_lists(list_BLOCKED_TCB, list_READY_TCB, tcb->parent_PID, tcb->TID, READY);
        pthread_mutex_unlock(&mutex_list_READY_TCB);
        pthread_mutex_unlock(&mutex_list_BLOCKED_TCB);
}



