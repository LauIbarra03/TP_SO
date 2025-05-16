#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "mutex_thread.h" // Aquí debes incluir tu código de mutex
#include <kernel.h>


/*static t_list *list_BLOCKED_TCB;
static t_list *list_READY_TCB;
static t_list *global_mutexes;

// Configuración inicial de las listas y mutexes
static int setup(void **state) {
    global_mutexes = list_create();
    list_BLOCKED_TCB = list_create();
    list_READY_TCB = list_create();
    return 0;
}

// Limpieza después de cada prueba
static int teardown(void **state) {
    list_destroy(global_mutexes);
    list_destroy(list_BLOCKED_TCB);
    list_destroy(list_READY_TCB);
    return 0;
}

// Test para crear un mutex
static void test_mutex_create(void **state) {

    t_pcb* pcb = create_process(10, "HOLA", 1);

    t_tcb* tcb= create_thread(pcb, "holita", 0);

    // Creación de un mutex llamado "mutex1" con un PCB asociado
    mutex_create("mutex1", &pcb);

    // Comprobamos que el mutex ha sido creado correctamente
    t_mutex *mutex = get_mutex("mutex1");
    assert_non_null(mutex);

    log_info(logger, "Mutex creado: %s", mutex->name);

    assert_string_equal(mutex->name, "mutex1");
    assert_int_equal(mutex->assigned_pid, 1234);
    assert_false(mutex->assigned); // El mutex no debe estar asignado todavía
}

// Test para bloquear un mutex
static void test_mutex_lock(void **state) {
    t_pcb pcb = { .PID = 1234 };
    t_tcb tcb = { .TID = 1, .parent_PID = 1234 };

    mutex_create("mutex1", &pcb);  // Crear mutex
    mutex_lock("mutex1", &tcb);    // Bloquear mutex con el TCB

    t_mutex *mutex = get_mutex("mutex1");
    assert_true(mutex->assigned);  // El mutex debe estar asignado
    assert_int_equal(mutex->assigned_tid, 1);  // El TID del hilo debe ser 1
}

// Test para desbloquear un mutex
static void test_mutex_unlock(void **state) {
    t_pcb pcb = { .PID = 1234 };
    t_tcb tcb = { .TID = 1, .parent_PID = 1234 };
    t_tcb tcb2 = { .TID = 2, .parent_PID = 1234 };

    mutex_create("mutex1", &pcb);  // Crear mutex
    mutex_lock("mutex1", &tcb);    // Bloquear mutex con el TCB

    // Simulamos que otro TCB se bloquea en el mutex
    mutex_lock("mutex1", &tcb2);

    // Desbloqueamos el primer TCB
    mutex_unlock("mutex1", &tcb);

    t_mutex *mutex = get_mutex("mutex1");
    assert_int_equal(mutex->assigned_tid, 2);  // El mutex debe ser asignado al siguiente TCB (TID 2)

    // Desbloqueamos el segundo TCB
    mutex_unlock("mutex1", &tcb2);

    // El mutex debe estar ahora libre
    assert_false(mutex->assigned);
}
*/