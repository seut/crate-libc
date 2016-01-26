//
//  StreamOutput.c
//  crate-libc
//
//  Created by smu on 25/01/16.
//  Copyright © 2016 smu. All rights reserved.
//

#include "streaming.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

char resizeBufferIfNeeded(StreamOutput* output);
void StreamOutput_increaseBufferPos(StreamOutput* output);

StreamOutput * StreamOutput_alloc(uint16_t bufferSize) {
    StreamOutput * streamOutput = malloc(sizeof(StreamOutput));
    if (!streamOutput) {
        return NULL;
    }
    memset(streamOutput, 0, sizeof(StreamOutput));

    streamOutput->bufferPos = 0;
    streamOutput->bufferSize = bufferSize;
    streamOutput->buffer = malloc(bufferSize);
    if (!streamOutput->buffer) {
        return NULL;
    }
    memset(streamOutput->buffer, 0, bufferSize);

    return streamOutput;
}

void StreamOutput_free(StreamOutput* output) {
    assert(output);
    if (output->buffer) {
        free(output->buffer);
    }
    free(output);
}

StreamOutput * StreamOutput_writeByte(StreamOutput* output, uint8_t byte) {
    if (resizeBufferIfNeeded(output)) {
        return NULL;
    }
    output->buffer[output->bufferPos] = byte;
    StreamOutput_increaseBufferPos(output);
    return output;
}

StreamOutput * StreamOutput_writeBoolean(StreamOutput* output, uint8_t boolean) {
    return StreamOutput_writeByte(output, boolean);
}

StreamOutput * StreamOutput_writeInt(StreamOutput* output, int32_t integer) {
    if (resizeBufferIfNeeded(output)) {
        return NULL;
    }
    *(uint32_t*) (output->buffer + output->bufferPos) = htonl(integer);
    StreamOutput_increaseBufferPos(output);
    return output;
}

StreamOutput * StreamOutput_writeVInt(StreamOutput* output, int32_t integer) {
    if (resizeBufferIfNeeded(output)) {
        return NULL;
    }

    while ((integer & ~0x7F) != 0) {
        *(int32_t*) (output->buffer + output->bufferPos) = ((integer & 0x7f) | 0x80);
        StreamOutput_increaseBufferPos(output);
        integer >>= 7;
    }
    *(int32_t*) (output->buffer + output->bufferPos) = integer;
    StreamOutput_increaseBufferPos(output);

    return output;
}


char resizeBufferIfNeeded(StreamOutput* output) {
    if (output->bufferPos + sizeof(uint8_t) > output->bufferSize) {
        output->buffer = realloc(output->buffer, output->bufferSize * 2);
        if (!output->buffer) {
            return 1;
        }
        output->bufferSize = output->bufferSize * 2;
    }
    return 0;
}

void StreamOutput_increaseBufferPos(StreamOutput* output) {
    output->bufferPos += sizeof(uint8_t);
}
