//
//  crate_common.h
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#ifndef crate_common_h
#define crate_common_h

#include <stdio.h>

#include "byte_reader.h"

typedef struct crate_netty_header_t {
    uint16_t msgSize;
    int64_t requestId;
    char status;
    char isResponse;
    char isError;
    char isCompressed;
    int32_t versionId;
} crate_netty_header;

crate_netty_header * crate_read_netty_header(crate_netty_header * nettyHeader, crate_buffer * buffer);

#endif /* crate_common_h */
