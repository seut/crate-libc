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

typedef struct StreamOutput_t
{
    uint16_t bufferSize;
    uint16_t bufferPos;
    uint16_t maxWrittenByte;
    uint8_t * buffer;
} StreamOutput;

StreamOutput *StreamOutput_alloc(uint32_t bufferSize);

void StreamOutput_free(StreamOutput *output);

StreamOutput *StreamOutput_writeByte(StreamOutput *output, uint8_t byte);

StreamOutput *StreamOutput_writeBoolean(StreamOutput *output, uint8_t boolean);

StreamOutput *StreamOutput_writeString(StreamOutput *output, uint8_t *string, uint32_t size);

StreamOutput *StreamOutput_writeInt(StreamOutput *output, int32_t integer);

StreamOutput *StreamOutput_writeVInt(StreamOutput *output, int32_t integer);

StreamOutput *StreamOutput_writeLong(StreamOutput *output, int64_t l);

StreamOutput *StreamOutput_writeVLong(StreamOutput *output, int64_t l);

StreamOutput *StreamOutput_skipBytes(StreamOutput* output, uint32_t skip);

StreamOutput *StreamOutput_reposition(StreamOutput* output, uint32_t pos);

// StreamInput

typedef struct StreamInput_t
{
    uint16_t bufferSize;
    uint16_t bufferPos;
    uint8_t *buffer;
} StreamInput;

StreamInput *StreamInput_alloc(uint8_t *buffer, uint16_t bufferSize);

void StreamInput_free(StreamInput *input);

uint8_t StreamInput_readByte(StreamInput *input);

uint8_t StreamInput_readBoolean(StreamInput *input);

uint8_t *StreamInput_readString(StreamInput *input);

int32_t StreamInput_readInt(StreamInput *input);

int32_t StreamInput_readVInt(StreamInput *input);

int64_t StreamInput_readLong(StreamInput *input);

int64_t StreamInput_readVLong(StreamInput *input);


#endif /* streaming_h */
