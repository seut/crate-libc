//
//  crate_request.c
//  crate-connect-c
//
//  Created by Adrian Partl on 25/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "crate_request.h"
#include "crate_common.h"
#include "byte_reader.h"

// SQL STATEMENT:
// FORMAT:
// NETTY_HEADER: 19
//      0: 'E'
//      1: 'S'
//      2: msg_size (write the size, the size indicates the remaining message size, not including the size int) (int)
//      6: request_id (long)
//      14: status (byte) - bitmask (0: request or response - 1: error or not - 2: compressed
//      15: version_id (int)
// 19: size action name (crate_sql)
// 20: action name: "crate_sql"
// ... a
// a + 0: expecting headers in payload? (bool) false (if true: x+1: headers map (not sure what and when this is required))
// a + 1: SQL statement (not 0 terminated!)
// ... b:
// b + 0: args length (VInt)
// b + ...: arguments list as Generic Values ()
// ... c:
// c + 0: creation time (VLong)
// ... d:
// d + 0: include types on response (bool)
// d + 1:

crate_request * crate_request_alloc(uint32_t size) {
    crate_request * request = malloc(sizeof(crate_request));
    if (!request) {
        return NULL;
    }
    memset(request, 0, sizeof(crate_request));
    
    request->buffer = crate_buffer_alloc(size);
    if (request->buffer == NULL) {
        return NULL;
    }
    
    request->nettyHeader.msgSize = size;
    
    return request;
}

void crate_request_free(crate_request * request) {
    assert(request);
    
    if (request->buffer) {
        crate_buffer_free(request->buffer);
        request->buffer = NULL;
    }
    
    if (request->action.action != NULL) {
        free(request->action.action);
        request->action.action = NULL;
    }
    
    if (request->payloadHeadersRaw != NULL) {
        free(request->payloadHeadersRaw);
        request->payloadHeadersRaw = NULL;
    }
    
    if (request->rawPayload != NULL) {
        free(request->rawPayload);
        request->rawPayload = NULL;
    }
    
    if (request->args != NULL) {
        free(request->args);
        request->args = NULL;
    }
    
    free(request);
}

crate_request * crate_read_request(crate_conn * connection, uint16_t size) {
    assert(connection);
    uint16_t len;
    
    crate_request * request = crate_request_alloc(size);
    if (request == NULL) {
        return NULL;
    }
    
    len = recv(connection->socket, request->buffer->buffer, size, 0);
    if (len != size) {
        return NULL;
    }
    request->buffer->pos = 0;
    
    if (crate_request_read_netty_header(request) == NULL) {
        crate_request_free(request);
        return NULL;
    }
    
    if (request->nettyHeader.isError) {
        printf("An error occurred. Geh scheissen!\n");
        return NULL;
    }
    
    if (crate_request_read_action(request) == NULL) {
        crate_request_free(request);
        return NULL;
    }

    if (crate_request_read_payload(request) == NULL) {
        crate_request_free(request);
        return NULL;
    }
    
    if (crate_request_read_arguments(request) == NULL) {
        crate_request_free(request);
        return NULL;
    }
    
    request->creationTime = read_Vlong(request->buffer);
    request->typesIncludedOnResponse = read_bool(request->buffer);
    
    return request;
}

crate_request * crate_request_read_netty_header(crate_request * request) {
    assert(request);
    
    crate_read_netty_header(&request->nettyHeader, request->buffer);
    
    return request;
}

crate_request * crate_request_read_action(crate_request * request) {
    assert(request);
    
    request->action.size = read_unsigned(request->buffer);
    
    request->action.action = read_bytes(request->buffer, request->action.size);
    if (request->action.action == NULL) {
        return NULL;
    }

    return request;
}

crate_request * crate_request_read_request_header(crate_request * request) {
    assert(request);
    
    request->hasPayloadHeaders = read_bool(request->buffer);
    
    if (request->hasPayloadHeaders) {
        // TODO: IMPLEMENT ME!
        return NULL;
    }
    
    return request;
}

crate_request * crate_request_read_payload(crate_request * request) {
    assert(request);
    
    request->rawPayload = read_string(request->buffer);
    if (request->rawPayload == NULL) {
        return NULL;
    }
    
    return request;
}

crate_request * crate_request_read_arguments(crate_request * request) {
    request->numArgs = read_Vint(request->buffer);
    
    if (request->numArgs > 0) {
        // TODO: IMPLEMENT ME!
        return NULL;
    }
    
    return request;
}

crate_request * crate_write_request(crate_conn * connection, char * sql) {
    assert(connection);
    
    crate_request * request = crate_request_alloc(0);
    if (request == NULL) {
        return NULL;
    }

    request->outBuffer = StreamOutput_alloc(100);
    if (request->outBuffer == NULL) {
        return NULL;
    }

    StreamOutput_skipBytes(request->outBuffer, 19);

    if (crate_request_write_action(request) == NULL) {
        return NULL;
    }

    if (crate_request_write_payload_header(request) == NULL) {
        return NULL;
    }

    if (crate_request_write_payload(request, (uint8_t *) sql, (uint32_t) strlen(sql)) == NULL) {
        return NULL;
    }

    if (crate_request_write_arguments(request) == NULL) {
        return NULL;
    }

    // TODO: FIX ME!
    if (StreamOutput_writeVLong(request->outBuffer, 666) == NULL) {
        return NULL;
    }

    // TODO: FIX ME!
    if (StreamOutput_writeByte(request->outBuffer, 0) == NULL) {
        return NULL;
    }

    if (crate_request_write_netty_header(request) == NULL) {
        return NULL;
    }

    return request;
}

crate_request * crate_request_write_netty_header(crate_request * request) {
    assert(request);

    StreamOutput_reposition(request->outBuffer, 0);
    StreamOutput_writeByte(request->outBuffer, 'E');
    StreamOutput_writeByte(request->outBuffer, 'S');
    StreamOutput_writeInt(request->outBuffer, request->outBuffer->maxWrittenByte - 6);
    // TODO: FIX ME! Generate proper request ID
    StreamOutput_writeLong(request->outBuffer, 1);
    StreamOutput_writeByte(request->outBuffer, 0);
    StreamOutput_writeInt(request->outBuffer, 1);

    return request;
}

crate_request * crate_request_write_action(crate_request * request) {
    assert(request);

    /*if (StreamOutput_writeByte(request->outBuffer, 9) == NULL) {
        return NULL;
    }*/
    
    if (StreamOutput_writeString(request->outBuffer, (uint8_t *) "crate_sql", 9) == NULL) {
        return NULL;
    }

    return request;
}

crate_request * crate_request_write_payload_header(crate_request * request) {
    assert(request);
    
    if (StreamOutput_writeByte(request->outBuffer, (uint8_t) 0) == NULL) {
        return NULL;
    }
    
    return request;
}

crate_request * crate_request_write_payload(crate_request * request, uint8_t * payload, uint32_t size) {
    assert(request);

    if (StreamOutput_writeString(request->outBuffer, payload, size) == NULL) {
        return NULL;
    }

    return request;
}

crate_request * crate_request_write_arguments(crate_request * request) {
    assert(request);

    if (StreamOutput_writeVInt(request->outBuffer, 0) == NULL) {
        return NULL;
    }

    return request;
}



