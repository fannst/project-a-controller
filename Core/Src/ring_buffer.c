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

#include "ring_buffer.h"

/// Adds one byte to the ring buffer.
RingBuffer_Err_t RingBuffer_AddByte (RingBuffer_t *buffer, uint8_t byte) {
    // Make sure we can add one byte.
    if (buffer == NULL)
        return RingBuffer_Err_NotAllocated;
    else if ((buffer->size + 1) > buffer->capacity)
        return RingBuffer_Err_Overflow;

    // Writes the byte to the current write pointer, and increment the write pointer.
    buffer->buffer[buffer->write] = byte;
    buffer->write = (buffer->write + 1) % buffer->capacity;

    // Increment the buffer size,a nd return OK.
    ++buffer->size;
    return RingBuffer_Err_OK;
}

/// Reads one byte from the ring buffer.
RingBuffer_Err_t RingBuffer_ReadByte (RingBuffer_t *buffer, uint8_t *byte) {
    // Make sure we can remove one byte.
    if (buffer == NULL)
        return RingBuffer_Err_NotAllocated;
    else if (buffer->read == buffer->write)
        return RingBuffer_Err_Underflow;

    // Read the byte from the current read position, and increment the read pointer.
    *byte = buffer->buffer[buffer->read];
    buffer->read = (buffer->read + 1) % buffer->capacity;
    
    // Decrement the buffer size, and return OK.
    --buffer->size;
    return RingBuffer_Err_OK;
}

/// Adds 'len' bytes to the ring buffer.
RingBuffer_Err_t RingBuffer_AddBytes (RingBuffer_t *buffer, const uint8_t *bytes, uint32_t len) {
    // Makes sure we can add N bytes.
    if (buffer == NULL)
        return RingBuffer_Err_NotAllocated;
    else if (buffer->size + len > buffer->capacity)
        return RingBuffer_Err_Overflow;

    // Reads N bytes from the buffer, and returns always OK.
    for (uint32_t i = 0; i < len; ++i)
        RingBuffer_AddByte (buffer, bytes[i]);

    return RingBuffer_Err_OK;
}

/// Reads 'len' bytes from the ring buffer.
RingBuffer_Err_t RingBuffer_ReadBytes (RingBuffer_t *buffer, uint8_t *ret, uint32_t len) {
    // Makes sure we can read N bytes.
    if (buffer == NULL)
        return RingBuffer_Err_NotAllocated;
    else if (len > buffer->size)
        return RingBuffer_Err_Underflow;

    // Reads N bytes and returns OK.
    for (uint32_t i = 0; i < len; ++i)
        RingBuffer_ReadByte (buffer, &ret[i]);
    
    return RingBuffer_Err_OK;
}

/// Peeks 'len' bytes form the ring buffer.
RingBuffer_Err_t RingBuffer_Peek (RingBuffer_t *buffer, uint8_t *ret, uint32_t len) {
    // Makes sure we can peek N bytes.
    if (buffer == NULL)
        return RingBuffer_Err_NotAllocated;
    else if (len > buffer->size)
        return RingBuffer_Err_Underflow;

    // Stores the old read, and the old size.
    uint32_t oldSize = buffer->size,
        oldRead = buffer->read;

    // Reads N bytes.
    for (uint32_t i = 0; i < len; ++i)
        RingBuffer_ReadByte (buffer, &ret[i]);
    
    // Restores the old read, and the old size and returns OK.
    buffer->size = oldSize;
    buffer->read = oldRead;

    return RingBuffer_Err_OK;
}

/// Creates a new ring buffer.
RingBuffer_Err_t RingBuffer_New (RingBuffer_t **ret, uint32_t size) {
    // Allocate the memory for the ring buffer structure.
    *ret = (RingBuffer_t *) malloc (sizeof (RingBuffer_t));
    if (*ret == NULL)
        return RingBuffer_Err_AllocationFailure;

    // Clear the ring buffer structure, and set the capacity.
    memset (*ret, 0, sizeof (RingBuffer_t));
    (*ret)->capacity = size;

    // Allocate the buffer space, if this fails free up ring buffer, and return error.
    (*ret)->buffer = malloc (size);
    if ((*ret)->buffer == NULL) {
        free (*ret);
        return RingBuffer_Err_AllocationFailure;
    }

    return RingBuffer_Err_OK;
}

/// Frees a ring buffer.
RingBuffer_Err_t RingBuffer_Free (RingBuffer_t **buffer) {
    // Makes sure the memory is actually allocated.
    if (*buffer == NULL)
        return RingBuffer_Err_NotAllocated;

    // First free the buffer pointer, after which we free the structure.
    free ((*buffer)->buffer);
    free (*buffer);

    // Set the ring buffer to NULL to prevent undefined behaviour, and return OK.
    *buffer = NULL;

    return RingBuffer_Err_OK;
}
