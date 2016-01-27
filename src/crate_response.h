//
//  crate_response.h
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#ifndef crate_response_h
#define crate_response_h

#include <stdio.h>

#include "crate_conn.h"
#include "crate_common.h"

typedef struct crate_response_t {
    crate_netty_header nettyHeader;
    char isNegative;
    int64_t rowCount;
    int32_t rowLength;
    int32_t colLength;
    int64_t creationTime;
    char typesIncluded;

    crate_buffer * buffer;
    crate_string_array * columns;

} crate_response;

crate_response * crate_response_alloc(uint32_t size);
void crate_response_free(crate_response * response);

crate_response * crate_read_response(crate_conn * connection, uint16_t size);

crate_response * crate_response_read_netty_header(crate_response * response);
crate_response * crate_response_read_is_empty(crate_response * response);
crate_response * crate_response_read_columns(crate_response * response);
crate_response * crate_response_read_rows(crate_response * response);
crate_response * crate_response_read_creation_time(crate_response * response);
crate_response * crate_response_read_types(crate_response * response);


#endif /* crate_response_h */
