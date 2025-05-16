//server.c

#include <string.h>
#include "server.h"

int start_server(const char* port) {
    int server_socket;

    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &server_info);

    // Create the server's listening socket

    for (p = server_info; p != NULL; p = p->ai_next) {
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket == -1) {
            continue;
        }

        int opt = 1;
        // To avoid the problem of listening on a client -1
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        // Bind the socket to a port
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == 0) {
            break;
        }

        close(server_socket);
    }

    if (p == NULL) {
        return -1;
    }

    // Listen for incoming connections
    freeaddrinfo(server_info);
    log_trace(logger, "Ready to listen to my client");
    listen(server_socket, 10);

    return server_socket;
}

int wait_for_client(int server_socket) {
    // Accept a new client
    int client_socket;

    struct sockaddr_in client_address;
    socklen_t address_size = sizeof(struct sockaddr_in);

    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_size);

    if (client_socket == -1) {
        perror("accept");
        return -1;
    }

    // log_info(logger, "A client connected!");

    return client_socket;
}

int receive_operation_code(int client_socket) {
    int op_code;
    if (recv(client_socket, &op_code, sizeof(int), MSG_WAITALL) > 0) {
        return op_code;
    } else {
        close(client_socket);
        return -1;
    }
}

void* receive_buffer(int* size, int client_socket) {
    void* buffer;
    recv(client_socket, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(client_socket, buffer, *size, MSG_WAITALL);

    return buffer;
}

void receive_message(int client_socket) {
    int size;
    char* buffer = receive_buffer(&size, client_socket);
    log_info(logger, "Message received: %s", buffer);
    free(buffer);
}

t_list* receive_package(int client_socket) {
    int size = 0;
    int offset = 0;
    void* buffer = NULL;
    t_list* values = list_create();
    int element_size;

    buffer = receive_buffer(&size, client_socket);
    while (offset < size) {
        memcpy(&element_size, buffer + offset, sizeof(int));
        offset += sizeof(int);
        char* value = malloc(element_size);
        memcpy(value, buffer + offset, element_size);
        offset += element_size;
        list_add(values, value);
    }

    free(buffer);
    return values;
}

void* receive_stream(int client_socket, int* total_size) {
    int bytes_received = 0;
    *total_size = 0;
    void* buffer = NULL;

    if (recv(client_socket, total_size, sizeof(int), MSG_WAITALL) <= 0) {
        perror("Error receiving total size");
        return NULL;
    }

    buffer = malloc(*total_size);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    while (bytes_received < *total_size) {
        int result = recv(client_socket, buffer + bytes_received, *total_size - bytes_received, MSG_WAITALL);
        if (result <= 0) {
            perror("Error receiving data");
            free(buffer);
            return NULL;
        }
        bytes_received += result;
    }

    return buffer;
}
