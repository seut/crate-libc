//
//  crate_conn.h
//  crate-connect-c
//
//  Created by Adrian Partl on 25/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#ifndef crate_conn_h
#define crate_conn_h

#include <stdio.h>

typedef struct crate_conn_t {
    int socket;
} crate_conn;


uint32_t crate_conn_read_msg_size(crate_conn * connection);



#endif /* crate_conn_h */
