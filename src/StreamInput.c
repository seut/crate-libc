//
//  StreamInput.c
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

uint8_t isEndOfBuffer(StreamInput *input);

void StreamInput_increaseBufferPos(StreamInput *input, size_t size);

#define READ_VINT_BYTE(input, tmp, out, shift) {\
    if (isEndOfBuffer(input)) {\
        return 0;\
    }\
    tmp = StreamInput_readByte(input);\
    out |= (int32_t)(tmp & 0x7F) << shift;\
    if ((tmp & 0x80) == 0) {\
        return out;\
    }\
}

#define READ_VLONG_BYTE(input, tmp, out, shift) {\
    if (isEndOfBuffer(input)) {\
        return 0;\
    }\
    tmp = StreamInput_readByte(input);\
    out |= (int64_t)(tmp & 0x7F) << shift;\
    if ((tmp & 0x80) == 0) {\
        return out;\
    }\
}

StreamInput *StreamInput_alloc(uint8_t *buffer, uint16_t bufferSize) {
    StreamInput *streamInput = malloc(sizeof(StreamInput));
    if (!streamInput) {
        return NULL;
    }
    memset(streamInput, 0, sizeof(StreamInput));

    streamInput->bufferPos = 0;
    streamInput->bufferSize = bufferSize;
    streamInput->buffer = buffer;

    return streamInput;
}

void StreamInput_free(StreamInput *input) {
    assert(input);
    free(input);
}

uint8_t StreamInput_readByte(StreamInput *input) {
    if (isEndOfBuffer(input)) {
        return 0;
    }
    uint8_t byte = input->buffer[input->bufferPos];
    StreamInput_increaseBufferPos(input, sizeof(uint8_t));
    return byte;
}

uint8_t StreamInput_readBoolean(StreamInput *input) {
    return StreamInput_readByte(input);
}

int32_t StreamInput_readInt(StreamInput *input) {
    if (isEndOfBuffer(input)) {
        return 0;
    }
    uint16_t offset = input->bufferPos;
    StreamInput_increaseBufferPos(input, sizeof(int32_t));
    return (int32_t) ntohl(*((uint32_t *) ((uint8_t *) (input->buffer + offset))));
}

int32_t StreamInput_readVInt(StreamInput *input) {
    int32_t output = 0;

    uint8_t b;
    READ_VINT_BYTE(input, b, output, 0);
    READ_VINT_BYTE(input, b, output, 7);
    READ_VINT_BYTE(input, b, output, 14);
    READ_VINT_BYTE(input, b, output, 21);

    b = StreamInput_readByte(input);

    assert((b & 0x80) == 0);
    output |= (int32_t) (b & 0x7F) << 28;

    return output;
}

int64_t StreamInput_readLong(StreamInput *input) {
    if (isEndOfBuffer(input)) {
        return 0;
    }
    uint16_t offset = input->bufferPos;
    StreamInput_increaseBufferPos(input, sizeof(int64_t));
    return (int64_t) ntohll(*((uint64_t *) ((uint8_t *) (input->buffer + offset))));
}

int64_t StreamInput_readVLong(StreamInput *input) {
    int64_t output = 0;

    uint8_t b;
    READ_VLONG_BYTE(input, b, output, 0);
    READ_VLONG_BYTE(input, b, output, 7);
    READ_VLONG_BYTE(input, b, output, 14);
    READ_VLONG_BYTE(input, b, output, 21);
    READ_VLONG_BYTE(input, b, output, 28);
    READ_VLONG_BYTE(input, b, output, 35);
    READ_VLONG_BYTE(input, b, output, 42);
    READ_VLONG_BYTE(input, b, output, 49);

    b = StreamInput_readByte(input);

    assert((b & 0x80) == 0);
    output |= (int64_t) (b & 0x7F) << 56;

    return output;
}


uint8_t *StreamInput_readString(StreamInput *input) {
    int32_t size = StreamInput_readVInt(input);

    StreamOutput *out = StreamOutput_alloc((uint32_t) size);
    uint32_t count = 0;
    uint8_t c;
    while (count < size) {
        c = (uint8_t) (StreamInput_readByte(input) & 0xff);
        switch (c >> 4) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                StreamOutput_writeByte(out, c);
                count++;
                break;
            case 12:
            case 13:
                StreamOutput_writeByte(out, (uint8_t) ((c & 0x1F) << 6 | (StreamInput_readByte(input) & 0x3F)));
                count++;
                break;
            case 14:
                StreamOutput_writeByte(out, (uint8_t) ((c & 0x0F) << 12 | (StreamInput_readByte(input) & 0x3F) << 6 |
                                                       (StreamInput_readByte(input) & 0x3F) << 0));
                count++;
                break;
        }
    }

    uint8_t *buffer = malloc(out->bufferSize + 1);
    if (buffer == NULL) {
        DEBUG_PRINT("Cannot allocate enough memory for reading a string");
        return NULL;
    }

    memcpy(buffer, out->buffer, out->bufferSize);
    buffer[out->bufferSize] = '\0';

    StreamOutput_free(out);
    return buffer;
}

float StreamInput_readFloat(StreamInput *input) {
    floatToIntBitwise convert;
    convert.i = StreamInput_readInt(input);
    return convert.f;
}

double StreamInput_readDouble(StreamInput *input) {
    doubleToLongBitwise convert;
    convert.l = StreamInput_readLong(input);
    return convert.d;
}


uint8_t isEndOfBuffer(StreamInput *input) {
    if (input->bufferPos > input->bufferSize) {
        return 1;
    }
    return 0;
}

void StreamInput_increaseBufferPos(StreamInput *input, size_t size) {
    input->bufferPos += size;
}
