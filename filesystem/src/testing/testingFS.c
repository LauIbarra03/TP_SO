#include "testingFS.h"

void test_dump_memory() {

    uint32_t memory_dump[] = {
        0x00000048, 0x00000045, 0x00000041, 0x00000056, 0x00000059,
        0x00000020, 0x00000049, 0x00000053, 0x00000020, 0x00000054,
        0x00000048, 0x00000045, 0x00000020, 0x00000043, 0x00000052,
        0x0000004F, 0x00000057, 0x0000004E, 0x00000021
    };

    void* datos = (void*) memory_dump;

    t_dump_context* contexto1 = malloc(sizeof(t_dump_context));
    contexto1->pid = 1;
    contexto1->tid = 0;
    contexto1->tamanio_proceso = 96;
    contexto1->datos = datos;

    uint32_t memory_dump_2[] = {
        0x0000004D, 0x00000049, 0x0000004C, 0x00000049, 0x00000054, 
        0x0000004F, 0x00000020, 0x00000050, 0x00000052, 0x00000045, 
        0x00000053, 0x00000049, 0x00000044, 0x00000045, 0x0000004E, 
        0x00000054, 0x00000045, 0x00000021
    };

    void* datos_2 = (void*) memory_dump_2;

    t_dump_context* contexto2 = malloc(sizeof(t_dump_context));
    contexto2->pid = 2;
    contexto2->tid = 1;
    contexto2->tamanio_proceso = 72;
    contexto2->datos = datos_2;   


    inicializar_fs();

    op_code codigo_retorno1 = execute_dump_memory(contexto1);

    op_code codigo_retorno2 = execute_dump_memory(contexto2);

    if(codigo_retorno1 == ERROR) {
        log_info(logger, "No se pudo hacer el dump");
        return;
    }

    //leer_primer_byte(bitmap_path);


    log_info(logger, "Se ejecuto bien el DUMP");
}
