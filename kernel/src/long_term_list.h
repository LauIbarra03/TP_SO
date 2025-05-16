#ifndef LONG_TERM_LIST_H_
#define LONG_TERM_LIST_H_

#include <semaphore.h>
#include <pthread.h>
#include <commons/log.h>
#include "tcb_lists.h"
#include <commons/collections/queue.h>

void init_long_term_scheduler();

bool try_create_NEW_process();

void try_create_NEW_process_loop();

#endif /* LONG_TERM_LIST_H_ */