#ifndef UTILS_MESSAGE_SERIALIZATION_H_
#define UTILS_MESSAGE_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>

typedef struct {
    u_int32_t length;
    char *message;
} t_message;

void serialize_message(t_message* message, t_buffer* buffer);

t_message* deserialize_message(void* stream);

t_message* create_message(const char* text, int size);

void serealize_paquete(t_paquete*);

#endif
