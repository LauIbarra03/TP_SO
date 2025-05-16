#include "instruction_serialization.h"

t_instruction* create_instruction(op_code name, t_list *params) {
    t_instruction* instruction = malloc(sizeof(t_instruction));
    if (instruction == NULL) {
        return NULL; // Manejo de errores
    }
    
    instruction->name = name;
    instruction->params = params;
    
    // Contar los parámetros, utilizando la función list_size
    instruction->params_count = list_size(params); 
    
    return instruction;
}

void serialize_instruction(t_instruction* instruction, t_buffer* buffer) {
    buffer->offset = 0;

    // Calcular el tamaño total necesario para el buffer
    size_t size = sizeof(op_code) + sizeof(u_int32_t);
    
    // Calcular el tamaño para cada parámetro
    for (size_t i = 0; i < instruction->params_count; i++) {
        size += strlen((char*)list_get(instruction->params, i)) + 1; // +1 para el terminador nulo
    }
    
    buffer->size = size;

    buffer->stream = malloc(size);
    if (buffer->stream == NULL) {
        return; // Manejo de errores
    }

    // Copiar el nombre de la operación
    memcpy(buffer->stream + buffer->offset, &(instruction->name), sizeof(op_code));
    buffer->offset += sizeof(op_code);

    // Copiar la cuenta de parámetros
    memcpy(buffer->stream + buffer->offset, &(instruction->params_count), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializar los parámetros
    for (size_t i = 0; i < instruction->params_count; i++) {
        char* param = (char*)list_get(instruction->params, i);
        size_t param_length = strlen(param) + 1; // +1 para el terminador nulo
        memcpy(buffer->stream + buffer->offset, param, param_length);
        buffer->offset += param_length;
    }
}

t_instruction* deserialize_instruction(void* stream) {
    t_instruction* instruction = malloc(sizeof(t_instruction));
    if (instruction == NULL) {
        return NULL; // Manejo de errores
    }

    int offset = 0;

    // Copiar el nombre de la operación
    memcpy(&(instruction->name), stream + offset, sizeof(op_code));
    offset += sizeof(op_code);

    // Copiar la cuenta de parámetros
    memcpy(&(instruction->params_count), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Crear la lista de parámetros
    instruction->params = list_create(); // Asume que tienes una función para crear t_list
    for (size_t i = 0; i < instruction->params_count; i++) {
        char* param = (char*)(stream + offset);
        size_t param_length = strlen(param) + 1; // +1 para el terminador nulo

        // Agregar el parámetro a la lista
        list_add(instruction->params, strdup(param)); // Duplica la cadena para almacenarla
        offset += param_length;
    }

    return instruction;
}

void destroy_instruction(t_instruction* instruction) {
    if (instruction != NULL) {
        if (instruction->params != NULL) {
            // Destruir la lista y liberar cada parámetro
            list_destroy_and_destroy_elements(instruction->params, free);
        }
        free(instruction);  // Liberar la estructura
    }
}