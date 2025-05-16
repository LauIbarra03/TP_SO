#include "kernel_interrupt.h"

extern sem_t mutex_kernel_interruption;
extern sem_t mutex_quantum_interruption;
extern sem_t sincro_interrupcion;

extern t_log *logger;

void interrupt_cpu(void* arg) {

    // log_warning(logger, "\n***********SE INICIALIZA INTERRUPT_CPU***********\n");

    t_config* config = (t_config*) arg;

    char* ip_cpu = config_get_string_value(config, "IP_CPU");
    char* puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");

    

    int server_cpu_interrupt_fd = create_connection(ip_cpu, puerto_cpu_interrupt);

    free(puerto_cpu_interrupt);

    while(1) {
        
        sem_wait(&sincro_interrupcion);

        log_debug(logger, "Interrumpiendo CPU");

        if(quantum_interruption) {

            op_code codigo_quantum = QUANTUM_INTERRUPTION;
            
            send(server_cpu_interrupt_fd, &codigo_quantum, sizeof(int), 0);

            log_warning(logger, "Se envió una interrupción de Quantum*");

            sem_wait(&mutex_quantum_interruption);
            quantum_interruption = 0;
            sem_post(&mutex_quantum_interruption);

        } else if(kernel_interruption) {
            op_code codigo_kernel = KERNEL_INTERRUPTION;
            send(server_cpu_interrupt_fd, &codigo_kernel, sizeof(int), 0);
            sem_wait(&mutex_kernel_interruption);
            kernel_interruption = 0;
            sem_post(&mutex_kernel_interruption);
        }
    }

    free_connection(server_cpu_interrupt_fd);
}
