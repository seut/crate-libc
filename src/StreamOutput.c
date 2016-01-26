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

uint8_t StreamOutput_resizeBufferIfNeeded(StreamOutput* output, size_t size);
void StreamOutput_increaseBufferPos(StreamOutput* output, size_t size);

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
    size_t size = sizeof(uint8_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    output->buffer[output->bufferPos] = byte;
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput * StreamOutput_writeBoolean(StreamOutput* output, uint8_t boolean) {
    return StreamOutput_writeByte(output, boolean);
}

StreamOutput * StreamOutput_writeInt(StreamOutput* output, int32_t integer) {
    size_t size = sizeof(int32_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(uint32_t*) (output->buffer + output->bufferPos) = htonl(integer);
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput * StreamOutput_writeVInt(StreamOutput* output, int32_t integer) {
    size_t size = sizeof(uint8_t);

    while ((integer & ~0x7F) != 0) {
        if (StreamOutput_resizeBufferIfNeeded(output, size)) {
            return NULL;
        }
        *(uint8_t*) (output->buffer + output->bufferPos) = ((integer & 0x7f) | 0x80);
        StreamOutput_increaseBufferPos(output, size);
        integer >>= 7;
    }
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(uint8_t*) (output->buffer + output->bufferPos) = integer;
    StreamOutput_increaseBufferPos(output, size);

    return output;
}

StreamOutput * StreamOutput_writeLong(StreamOutput* output, int64_t l) {
    size_t size = sizeof(int64_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(uint64_t*) (output->buffer + output->bufferPos) = htonll(l);
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput * StreamOutput_writeVLong(StreamOutput* output, int64_t l) {
    size_t size = sizeof(uint8_t);
    while ((l & ~0x7F) != 0) {
        if (StreamOutput_resizeBufferIfNeeded(output, size)) {
            return NULL;
        }
        *(uint8_t*) (output->buffer + output->bufferPos) = ((l & 0x7f) | 0x80);
        StreamOutput_increaseBufferPos(output, size);
        l >>= 7;
    }
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(uint8_t*) (output->buffer + output->bufferPos) = l;
    StreamOutput_increaseBufferPos(output, size);

    return output;
}

uint8_t StreamOutput_resizeBufferIfNeeded(StreamOutput* output, size_t size) {
    while (output->bufferPos + size > output->bufferSize) {
        output->buffer = realloc(output->buffer, output->bufferSize * 2);
        if (!output->buffer) {
            return 1;
        }
        output->bufferSize = output->bufferSize * 2;
    }
    return 0;
}

void StreamOutput_increaseBufferPos(StreamOutput* output, size_t size) {
    output->bufferPos += size;
}
