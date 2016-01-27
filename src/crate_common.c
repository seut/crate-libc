//
//  crate_common.c
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#include <assert.h>

#include "crate_common.h"

crate_netty_header * crate_read_netty_header(crate_netty_header * nettyHeader, crate_buffer * buffer) {
    assert(nettyHeader);
    
    nettyHeader->requestId = read_long(buffer);
    nettyHeader->status = read_bool(buffer);
    nettyHeader->versionId = read_int(buffer);
    
    nettyHeader->isResponse = (nettyHeader->status & 0x01) != 0;
    nettyHeader->isError = (nettyHeader->status & 0x02) != 0;
    nettyHeader->isCompressed = (nettyHeader->status & 0x04) != 0;
    
    return nettyHeader;
}