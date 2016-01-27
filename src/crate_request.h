//
//  crate_request.h
//  crate-connect-c
//
//  Created by Adrian Partl on 25/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#ifndef crate_request_h
#define crate_request_h

#include <stdio.h>

#include "crate_conn.h"
#include "byte_reader.h"
#include "crate_common.h"
#include "streaming.h"

typedef struct crate_action_t {
    unsigned size;
    char * action;
} crate_action;

typedef struct crate_request_t {
    crate_netty_header nettyHeader;
    crate_action action;
    char hasPayloadHeaders;
    uint16_t numArgs;
    int64_t creationTime;
    char typesIncludedOnResponse;
    
    crate_buffer * buffer;
    StreamOutput * outBuffer;
    char * payloadHeadersRaw;
    char * rawPayload;
    char * args;
    
} crate_request;

crate_request * crate_request_alloc(uint32_t size);
void crate_request_free(crate_request * request);

crate_request * crate_read_request(crate_conn * connection, uint16_t size);

crate_request * crate_request_read_netty_header(crate_request * request);
crate_request * crate_request_read_action(crate_request * request);
crate_request * crate_request_read_payload_header(crate_request * request);
crate_request * crate_request_read_payload(crate_request * request);
crate_request * crate_request_read_arguments(crate_request * request);

crate_request * crate_write_request(crate_conn * connection, char * sql);
crate_request * crate_request_write_netty_header(crate_request * request);
crate_request * crate_request_write_action(crate_request * request);
crate_request * crate_request_write_payload_header(crate_request * request);
crate_request * crate_request_write_payload(crate_request * request, uint8_t * payload, uint32_t size);
crate_request * crate_request_write_arguments(crate_request * request);


#endif /* crate_request_h */
