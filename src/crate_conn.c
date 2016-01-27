//
//  crate_conn.c
//  crate-connect-c
//
//  Created by Adrian Partl on 25/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "crate_conn.h"
#include "byte_reader.h"

uint32_t crate_conn_read_msg_size(crate_conn * connection) {
    assert(connection != NULL);
    
    char recBuffer[6];
    memset(recBuffer, 0, 6);
    
    uint16_t len = recv(connection->socket, recBuffer, 6, 0);
    if (len != 6) {
        return 0;
    }
    
    if (recBuffer[0] != 'E' || recBuffer[1] != 'S') {
        return 0;
    }

    return ntohl( *( (uint32_t*)( (char*)(recBuffer + 2) ) ) );
}