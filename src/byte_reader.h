//
//  byte_reader.h
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#ifndef byte_reader_h
#define byte_reader_h

#include <stdio.h>
#include <stdint.h>

typedef struct crate_buffer_t {
    uint32_t size;
    uint32_t pos;
    char * buffer;
} crate_buffer;

typedef struct crate_string_array_t {
    uint32_t size;
    char ** elements;
} crate_string_array;

crate_buffer * crate_buffer_alloc(uint32_t size);
void crate_buffer_free(crate_buffer * buffer);

void crate_string_array_free(crate_string_array * array);

uint32_t read_uint32(crate_buffer * buffer);

char read_bool(crate_buffer * buffer);

unsigned read_unsigned(crate_buffer * buffer);
int32_t read_int(crate_buffer * buffer);
int64_t read_long(crate_buffer * buffer);

int32_t read_Vint(crate_buffer * buffer);
int64_t read_Vlong(crate_buffer * buffer);

char * read_bytes(crate_buffer * buffer, uint32_t length);
char * read_string(crate_buffer * buffer);

crate_string_array * read_string_array(crate_buffer * buffer);



#endif /* byte_reader_h */
