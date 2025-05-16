//communication.c

#include "communication.h"
#include <string.h>

void* serialize_package(t_paquete *paquete, int bytes) {
    
    bytes = sizeof(int) + sizeof(uint32_t) + paquete->buffer->size;
    void *magic = malloc(bytes);
    int offset = 0;

    memcpy(magic + offset, &(paquete->operation_code), sizeof(int));
    offset += sizeof(int);
    memcpy(magic + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(magic + offset, paquete->buffer->stream, paquete->buffer->size);
    offset += paquete->buffer->size;

    return magic;
}

t_paquete* deserealizar_paquete(void* stream) {
    t_paquete* paquete_nuevo = malloc(sizeof(t_paquete));
    int offset = 0;

    memcpy(&(paquete_nuevo->operation_code), stream + offset, sizeof(int));
    offset += sizeof(int);

    uint32_t buffer_size;
    memcpy(&buffer_size, stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    paquete_nuevo->buffer = malloc(sizeof(t_buffer));
    paquete_nuevo->buffer->size = buffer_size;
    paquete_nuevo->buffer->stream = malloc(buffer_size);

    memcpy(&(paquete_nuevo->buffer->stream), stream + offset, paquete_nuevo->buffer->size);

    return paquete_nuevo;    
}

int create_connection(char *ip, char *port) {
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, port, &hints, &server_info);

    int client_socket = 0;

    struct addrinfo *p;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(client_socket);
            continue;
        }

        break;
    }

    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(server_info);

    return client_socket;
}

int connection_by_config(t_config *config, char *ip_config, char *port_config) {
    char *ip = config_get_string_value(config, ip_config);
    char *port = config_get_string_value(config, port_config);
    return create_connection(ip, port);
}

void send_message(char *message, int client_socket) {
    t_paquete *package = malloc(sizeof(t_paquete));

    package->operation_code = MESSAGE;
    package->buffer = malloc(sizeof(t_buffer));
    package->buffer->size = strlen(message) + 1;
    package->buffer->stream = malloc(package->buffer->size);
    memcpy(package->buffer->stream, message, package->buffer->size);

    int bytes = package->buffer->size + 2 * sizeof(int);

    void *to_send = serialize_package(package, bytes);

    send(client_socket, to_send, bytes, 0);

    free(to_send);
    delete_package(package);
}

void create_buffer(t_paquete *package) {
    package->buffer = malloc(sizeof(t_buffer));
    package->buffer->size = 0;
    package->buffer->stream = NULL;
}

t_paquete *create_package(op_code op) {
    t_paquete *package = malloc(sizeof(t_paquete));
    package->operation_code = op;
    create_buffer(package);
    return package;
}

void add_to_package(t_paquete *package, void *value, int size) {
    package->buffer->stream = realloc(package->buffer->stream, package->buffer->size + size + sizeof(int));

    memcpy(package->buffer->stream + package->buffer->size, &size, sizeof(int));
    memcpy(package->buffer->stream + package->buffer->size + sizeof(int), value, size);

    package->buffer->size += size + sizeof(int);
}

void send_package(t_paquete *package, int client_socket) {
    int bytes = package->buffer->size + 2 * sizeof(int);
    void *to_send = serialize_package(package, bytes);

    send(client_socket, to_send, bytes, 0);

    free(to_send);
}

void delete_package(t_paquete *package) {
    free(package->buffer->stream);
    free(package->buffer);
    free(package);
}

void free_connection(int client_socket) {
    close(client_socket);
}

void send_response(int client_socket, op_code code) {
    send(client_socket, &code, sizeof(code), 0);
}

op_code wait_for_response(int client_socket) {
    op_code code;
    recv(client_socket, &code, sizeof(op_code), 0);
    return code;
}

char *receive_memory_response(int socket_cliente) {
    receive_operation_code(socket_cliente);
    int size;
    char* instruction = receive_buffer(&size, socket_cliente);
    return instruction;
}

void destroy_buffer(t_buffer* buffer) {
    free(buffer->stream);
    free(buffer);
}
