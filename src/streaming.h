//
//  streaming.h
//  crate-libc
//
//  Created by smu on 25/01/16.
//  Copyright © 2016 smu. All rights reserved.
//

#ifndef streaming_h
#define streaming_h

#include <stdio.h>
#include <stdint.h>

// StreamOutput

struct StreamOutput
{
    uint16_t bufferSize;
    uint16_t bufferPos;
    uint8_t * buffer;
};
typedef struct StreamOutput StreamOutput;

StreamOutput * StreamOutput_alloc(uint16_t bufferSize);

void StreamOutput_free(StreamOutput* output);

StreamOutput * StreamOutput_writeByte(StreamOutput* output, uint8_t byte);

StreamOutput * StreamOutput_writeBoolean(StreamOutput* output, uint8_t boolean);

StreamOutput * StreamOutput_writeInt(StreamOutput* output, int32_t integer);

StreamOutput * StreamOutput_writeVInt(StreamOutput* output, int32_t integer);


// StreamInput

struct StreamInput
{
    uint16_t bufferSize;
    uint16_t bufferPos;
    uint8_t * buffer;
};
typedef struct StreamInput StreamInput;

StreamInput * StreamInput_alloc(uint8_t * buffer, uint16_t bufferSize);

void StreamInput_free(StreamInput* input);

char StreamInput_readByte(StreamInput* input);

char StreamInput_readBoolean(StreamInput* input);

char * StreamInput_readString(StreamInput* input);

int32_t StreamInput_readInt(StreamInput* input);

int32_t StreamInput_readVInt(StreamInput* input);

int64_t StreamInput_readLong(StreamInput* input);

int64_t StreamInput_readVLong(StreamInput* input);


#endif /* streaming_h */
