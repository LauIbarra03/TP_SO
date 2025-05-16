#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>
#include <utils/message_serialization.h>

void serialize_message(t_message* message, t_buffer* buffer) {
    buffer->offset = 0;
    
    // Calculamos el tamaño total del buffer: tamaño del length + el mensaje
    size_t size = sizeof(u_int32_t) + message->length * sizeof(char);
    buffer->size = size;

    // Asignamos memoria para el stream del buffer
    buffer->stream = malloc(size);

    // Serializamos la longitud del mensaje
    memcpy(buffer->stream + buffer->offset, &(message->length), sizeof(u_int32_t));
    buffer->offset += sizeof(u_int32_t);

    // Serializamos el contenido del mensaje
    memcpy(buffer->stream + buffer->offset, message->message, message->length * sizeof(char));
    buffer->offset += message->length * sizeof(char);
}

t_message* deserialize_message(void* stream) {
    t_message* message = malloc(sizeof(t_message));
    int offset = 0;

    // Deserializar la longitud del mensaje
    memcpy(&(message->length), stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);

    // Asignar memoria para el contenido del mensaje basado en la longitud
    message->message = malloc(message->length * sizeof(char));

    // Deserializar el contenido del mensaje
    memcpy(message->message, stream + offset, message->length * sizeof(char));
    offset += message->length * sizeof(char);

    return message;
}

t_message* create_message(const char* text, int size) {
    t_message* msg = malloc(sizeof(t_message));  // Asignamos memoria para el struct
    if (msg == NULL) {
        log_error(logger, "Error al alocar memoria para el mensaje");
        exit(EXIT_FAILURE);
    }
    
    msg->length = size;  // Usamos el tamaño pasado como parámetro
    msg->message = malloc(msg->length * sizeof(char));  // Asignamos memoria para el mensaje
    
    if (msg->message == NULL) {
        log_error(logger, "Error al alocar memoria para el mensaje");
        free(msg);
        exit(EXIT_FAILURE);
    }
    
    strncpy(msg->message, text, msg->length);  // Copiamos el mensaje en la estructura
    return msg;
}
