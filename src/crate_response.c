//
//  crate_response.c
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "crate_response.h"
#include "crate_common.h"
#include "byte_reader.h"

// SQL RESPONSE:
// FORMAT:
// NETTY_HEADER: 19
//      0: 'E'
//      1: 'S'
//      2: msg_size (write the size, the size indicates the remaining message size, not including the size int) (int)
//      6: request_id (long)
//      14: status (byte) - bitmask (0: request or response - 1: error or not - 2: compressed
//      15: version_id (int)
// 19: false
// 20: isEmpty (bool)
// 21: rowCount (Vlong)
// ... a
// a + 0: columns (String array)
// ... b
// b + 0: row length (int)
// ... : ROWS with cols
// ... c
// c + 0: request start time (Vlong)
// ... d
// d + 0: include types (bool)
// d + 1: (if include types = true): column types length (int)
// d + 5: DATATYPES

crate_response * crate_response_alloc(uint32_t size) {
    crate_response * response = malloc(sizeof(crate_response));
    if (!response) {
        return NULL;
    }
    memset(response, 0, sizeof(crate_response));
    
    response->buffer = crate_buffer_alloc(size);
    if (response->buffer == NULL) {
        return NULL;
    }
    
    response->nettyHeader.msgSize = size;
    
    return response;
}

void crate_response_free(crate_response * response) {
    assert(response);
    
    if (response->buffer) {
        crate_buffer_free(response->buffer);
        response->buffer = NULL;
    }
    
    if (response->columns) {
        crate_string_array_free(response->columns);
    }
    
    free(response);
}

crate_response * crate_read_response(crate_conn * connection, uint16_t size) {
    assert(connection);
    uint16_t len;
    
    crate_response * response = crate_response_alloc(size);
    if (response == NULL) {
        return NULL;
    }
    
    len = recv(connection->socket, response->buffer->buffer, size, 0);
    if (len != size) {
        return NULL;
    }
    response->buffer->pos = 0;
    
    if (crate_response_read_netty_header(response) == NULL) {
        crate_response_free(response);
        return NULL;
    }
    
    if (response->nettyHeader.isError) {
        printf("An error occurred. Geh scheissen!\n");
        return NULL;
    }

    crate_response_read_is_empty(response);
    
    if (crate_response_read_columns(response) == NULL) {
        return NULL;
    }
    
    if (response->isNegative == 1) {
        // TODO: IMPLEMENT ME!
        return NULL;
    }
    
    if (crate_response_read_rows(response) == NULL) {
        return NULL;
    }
    
    crate_response_read_creation_time(response);
    
    if (response->typesIncluded) {
        //TODO: IMPLEMENT ME!
        return NULL;
    }
    
    return response;
}

crate_response * crate_response_read_netty_header(crate_response * response) {
    assert(response);
    
    crate_read_netty_header(&response->nettyHeader, response->buffer);
    
    return response;
}

crate_response * crate_response_read_is_empty(crate_response * response) {
    assert(response);
    
    read_bool(response->buffer);
    response->isNegative = read_bool(response->buffer);
    
    return response;
}

crate_response * crate_response_read_columns(crate_response * response) {
    assert(response);
    
    response->rowCount = read_Vlong(response->buffer);
    response->columns = read_string_array(response->buffer);
    if (response->columns->size != 0 && response->columns == NULL) {
        return NULL;
    }
    
    return response;
}

crate_response * crate_response_read_rows(crate_response * response) {
    assert(response);
    
    response->rowLength = read_int(response->buffer);
    
    if (response->rowLength > 0) {
        // TODO: IMPLEMENT ME!
        return NULL;
    }
    
    return response;
}

crate_response * crate_response_read_creation_time(crate_response * response) {
    assert(response);
    
    response->creationTime = read_Vlong(response->buffer);
    response->typesIncluded = read_bool(response->buffer);
    
    return response;
}

crate_response * crate_response_read_types(crate_response * response) {
    return NULL;
}




