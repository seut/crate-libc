//
//  byte_reader.c
//  crate-connect-c
//
//  Created by Adrian Partl on 26/01/16.
//  Copyright © 2016 crate.io. All rights reserved.
//

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "byte_reader.h"

crate_buffer * crate_buffer_alloc(uint32_t size) {
    crate_buffer * buffer = malloc(sizeof(crate_buffer));
    if (buffer == NULL) {
        return NULL;
    }
    memset(buffer, 0, sizeof(crate_buffer));
    
    buffer->pos = 0;
    buffer->buffer = malloc(size);
    if (buffer->buffer == NULL) {
        free(buffer);
        return NULL;
    }
    memset(buffer->buffer, 0, size);
    buffer->size = size;
    
    return buffer;
}

void crate_buffer_free(crate_buffer * buffer) {
    assert(buffer);
    
    if (buffer->buffer) {
        free(buffer->buffer);
    }
    
    free(buffer);    
}

uint32_t read_uint32(crate_buffer * buffer) {
    uint32_t offset = buffer->pos;
    buffer->pos += sizeof(uint32_t);
    return ntohl( *( (uint32_t*)( (char*)(buffer->buffer + offset) ) ) );
}

char read_bool(crate_buffer * buffer) {
    uint32_t offset = buffer->pos;
    buffer->pos += sizeof(char);
    return *( (char*)(buffer->buffer + offset) );
}

unsigned read_unsigned(crate_buffer * buffer) {
    uint32_t offset = buffer->pos;
    buffer->pos += sizeof(unsigned);
    return (unsigned) buffer->buffer[offset];
}

int32_t read_int(crate_buffer * buffer) {
    uint32_t offset = buffer->pos;
    buffer->pos += sizeof(uint32_t);
    return (int32_t) ntohl( *( (uint32_t*)( (char*)(buffer->buffer + offset) ) ) );
}

int64_t read_long(crate_buffer * buffer) {
    uint32_t offset = buffer->pos;
    buffer->pos += sizeof(uint64_t);
    return (int64_t) ntohll( *( (uint64_t*)( (char*)(buffer->buffer + offset) ) ) );
}

#define READ_VINT_BYTE(buffer, tmp, out, shift) {\
    tmp = *(char*)(buffer->buffer + buffer->pos);\
    buffer->pos++;\
    out |= (int32_t)(tmp & 0x7F) << shift;\
    if ((tmp & 0x80) == 0) {\
        return out;\
    }\
}

int32_t read_Vint(crate_buffer * buffer) {
    int32_t output = 0;
    
    char b;
    READ_VINT_BYTE(buffer, b, output, 0);
    READ_VINT_BYTE(buffer, b, output, 7);
    READ_VINT_BYTE(buffer, b, output, 14);
    READ_VINT_BYTE(buffer, b, output, 21);
    
    b = *(char*)(buffer->buffer + buffer->pos);
    buffer->pos++;
    assert((b & 0x80) == 0);
    output |= (int32_t)(b & 0x7F) << 28;
    
    return output;
}

#define READ_VLONG_BYTE(buffer, tmp, out, shift) {\
    tmp = *(char*)(buffer->buffer + buffer->pos);\
    buffer->pos++;\
    out |= (int64_t)(tmp & 0x7F) << shift;\
    if ((tmp & 0x80) == 0) {\
        return out;\
    }\
}

int64_t read_Vlong(crate_buffer * buffer) {
    int64_t output = 0;
    
    char b;
    READ_VLONG_BYTE(buffer, b, output, 0);
    READ_VLONG_BYTE(buffer, b, output, 7);
    READ_VLONG_BYTE(buffer, b, output, 14);
    READ_VLONG_BYTE(buffer, b, output, 21);
    READ_VLONG_BYTE(buffer, b, output, 28);
    READ_VLONG_BYTE(buffer, b, output, 35);
    READ_VLONG_BYTE(buffer, b, output, 42);
    READ_VLONG_BYTE(buffer, b, output, 49);

    b = *(char*)(buffer->buffer + buffer->pos);
    buffer->pos++;
    assert((b & 0x80) == 0);
    output |= (int64_t)(b & 0x7F) << 56;

    return output;
}

char * read_bytes(crate_buffer * buffer,  uint32_t length) {
    uint32_t offset = buffer->pos;

    char * output = NULL;
    // plus one for zero terminating char array
    output = malloc(length + 1);
    if (output == NULL) {
        return NULL;
    }
    memset(output, 0, length);
    memcpy(output, (char*)(buffer + offset), length);
    buffer->pos += length;
    
    return output;
}

char * read_string(crate_buffer * buffer) {
    int32_t length = read_Vint(buffer);
    
    char * output = read_bytes(buffer, length);
    return output;    
}

void crate_string_array_free(crate_string_array * array) {
    assert(array);
    
    if (array->elements != NULL) {
        for (int i; i < array->size; i++) {
            if (array->elements[i] != NULL) {
                free(array->elements[i]);
            }
        }
        free(array->elements);
    }
}

crate_string_array * read_string_array(crate_buffer * buffer) {
    crate_string_array * array = malloc(sizeof(crate_string_array));
    if (array == NULL) {
        return NULL;
    }
    memset(array, 0, sizeof(crate_string_array));
    
    array->size = read_Vint(buffer);
    
    array->elements = malloc(array->size * sizeof(char*));
    if (array->size != 0 && array->elements == NULL) {
        crate_string_array_free(array);
        return NULL;
    }
    
    for (int i; i < array->size; i++) {
        array->elements[i] = read_string(buffer);
        if (array->elements[i] == NULL) {
            crate_string_array_free(array);
            return NULL;
        }
    }
    
    return array;
}


