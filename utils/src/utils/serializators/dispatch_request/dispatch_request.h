#ifndef UTILS_DISPATCH_REQUEST_SERIALIZATION_H_
#define UTILS_DISPATCH_REQUEST_SERIALIZATION_H_

#include <stdlib.h>
#include <stdio.h>

#include <utils/server.h>
#include <utils/communication.h>

typedef struct {
    u_int32_t tid;
    u_int32_t pid;
} t_dispatch_request;


void serialize_dispatch_request(t_dispatch_request* request, t_buffer* buffer) ;

t_dispatch_request* deserialize_dispatch_request(void* stream);

t_dispatch_request* create_dispatch_request(u_int32_t tid, u_int32_t pid);

void destroy_dispatch_request(t_dispatch_request* request);


#endif

