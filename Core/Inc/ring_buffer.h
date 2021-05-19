/*
Copyright 2021 Luke A.C.A. Rieff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RingBuffer_Err_Overflow,
    RingBuffer_Err_Underflow,
    RingBuffer_Err_OK,
    RingBuffer_Err_AllocationFailure,
    RingBuffer_Err_NotAllocated
} RingBuffer_Err_t;

typedef struct {
    uint32_t read, write, size, capacity;
    uint8_t *buffer;
} RingBuffer_t;

/// Adds one byte to the ring buffer.
RingBuffer_Err_t RingBuffer_AddByte (RingBuffer_t *buffer, uint8_t byte);

/// Reads one byte from the ring buffer.
RingBuffer_Err_t RingBuffer_ReadByte (RingBuffer_t *buffer, uint8_t *byte);

/// Adds 'len' bytes to the ring buffer.
RingBuffer_Err_t RingBuffer_AddBytes (RingBuffer_t *buffer, const uint8_t *bytes, uint32_t len);

/// Reads 'len' bytes from the ring buffer.
RingBuffer_Err_t RingBuffer_ReadBytes (RingBuffer_t *buffer, uint8_t *ret, uint32_t len);

/// Peeks 'len' bytes form the ring buffer.
RingBuffer_Err_t RingBuffer_Peek (RingBuffer_t *buffer, uint8_t *ret, uint32_t len);

/// Creates a new ring buffer.
RingBuffer_Err_t RingBuffer_New (RingBuffer_t **ret, uint32_t size);

/// Frees a ring buffer.
RingBuffer_Err_t RingBuffer_Free (RingBuffer_t **buffer);

#ifdef __cplusplus
}
#endif

#endif
