#ifndef KERNEL_KERNEL_INTERRUPT_H
#define KERNEL_KERNEL_INTERRUPT_H


#include <kernel.h>
#include <utils/communication.h>
#include <utils/estructuras.h>

void interrupt_cpu(void* arg);

#endif