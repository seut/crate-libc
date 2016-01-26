//
//  StreamOutput.c
//  crate-libc
//
//  Created by smu on 25/01/16.
//  Copyright © 2016 smu. All rights reserved.
//

#include "streaming.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

uint8_t StreamOutput_resizeBufferIfNeeded(StreamOutput *output, size_t size);

void StreamOutput_increaseBufferPos(StreamOutput *output, size_t size);

StreamOutput *StreamOutput_alloc(uint32_t bufferSize) {
    StreamOutput *streamOutput = malloc(sizeof(StreamOutput));
    if (!streamOutput) {
        DEBUG_PRINT("Cannot allocate enough memory for creating output streamer");
        return NULL;
    }
    memset(streamOutput, 0, sizeof(StreamOutput));

    streamOutput->bufferPos = 0;
    streamOutput->bufferSize = bufferSize;
    streamOutput->buffer = malloc(bufferSize);
    if (!streamOutput->buffer) {
        DEBUG_PRINT("Cannot allocate enough memory for writing to output buffer");
        return NULL;
    }
    memset(streamOutput->buffer, 0, bufferSize);

    return streamOutput;
}

void StreamOutput_free(StreamOutput *output) {
    assert(output);
    if (output->buffer) {
        free(output->buffer);
    }
    free(output);
}

StreamOutput *StreamOutput_writeByte(StreamOutput *output, uint8_t byte) {
    size_t size = sizeof(uint8_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    output->buffer[output->bufferPos] = byte;
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput *StreamOutput_writeBoolean(StreamOutput *output, uint8_t boolean) {
    return StreamOutput_writeByte(output, boolean);
}

StreamOutput *StreamOutput_writeInt(StreamOutput *output, int32_t integer) {
    size_t size = sizeof(int32_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(int32_t *) (output->buffer + output->bufferPos) = htonl(integer);
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput *StreamOutput_writeVInt(StreamOutput *output, int32_t integer) {
    size_t size = sizeof(uint8_t);

    while ((integer & ~0x7F) != 0) {
        if (StreamOutput_resizeBufferIfNeeded(output, size)) {
            return NULL;
        }
        *(int32_t *) (output->buffer + output->bufferPos) = ((integer & 0x7f) | 0x80);
        StreamOutput_increaseBufferPos(output, size);
        integer >>= 7;
    }
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(int32_t *) (output->buffer + output->bufferPos) = integer;
    StreamOutput_increaseBufferPos(output, size);

    return output;
}

StreamOutput *StreamOutput_writeLong(StreamOutput *output, int64_t l) {
    size_t size = sizeof(int64_t);
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(int64_t *) (output->buffer + output->bufferPos) = htonll(l);
    StreamOutput_increaseBufferPos(output, size);
    return output;
}

StreamOutput *StreamOutput_writeVLong(StreamOutput *output, int64_t l) {
    size_t size = sizeof(uint8_t);
    while ((l & ~0x7F) != 0) {
        if (StreamOutput_resizeBufferIfNeeded(output, size)) {
            return NULL;
        }
        *(int64_t *) (output->buffer + output->bufferPos) = ((l & 0x7f) | 0x80);
        StreamOutput_increaseBufferPos(output, size);
        l >>= 7;
    }
    if (StreamOutput_resizeBufferIfNeeded(output, size)) {
        return NULL;
    }
    *(int64_t *) (output->buffer + output->bufferPos) = l;
    StreamOutput_increaseBufferPos(output, size);

    return output;
}

StreamOutput *StreamOutput_writeString(StreamOutput *output, uint8_t *string, uint32_t size) {
    StreamOutput_writeVInt(output, size);

    uint8_t c;
    for (uint32_t i = 0; i < size; i++) {
        c = string[i];
        if (c <= 0x007F) {
            StreamOutput_writeByte(output, c);
        } else if (c > 0x07FF) {
            StreamOutput_writeByte(output, (uint8_t) (0xE0 | c >> 12 & 0x0F));
            StreamOutput_writeByte(output, (uint8_t) (0x80 | c >> 6 & 0x3F));
            StreamOutput_writeByte(output, (uint8_t) (0x80 | c >> 0 & 0x3F));
        } else {
            StreamOutput_writeByte(output, (uint8_t) (0xC0 | c >> 6 & 0x1F));
            StreamOutput_writeByte(output, (uint8_t) (0x80 | c >> 0 & 0x3F));
        }
    }

    return output;
}


uint8_t StreamOutput_resizeBufferIfNeeded(StreamOutput *output, size_t size) {
    while (output->bufferPos + size > output->bufferSize) {
        uint8_t newBufferSize = (uint8_t) ((output->bufferSize * 2) + 1);
        DEBUG_PRINT("Resizing buffer from %d to %d\n", output->bufferSize, newBufferSize);
        output->buffer = realloc(output->buffer, newBufferSize);
        if (!output->buffer) {
            DEBUG_PRINT("Resizing buffer failed!");
            return 1;
        }
        output->bufferSize = newBufferSize;
    }
    return 0;
}

void StreamOutput_increaseBufferPos(StreamOutput *output, size_t size) {
    output->bufferPos += size;
}
