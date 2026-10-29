/**
 * @file  ringbuf.h
 * @brief Lock-free single-producer / single-consumer byte ring buffer.
 *
 * Head is written only by the producer (e.g. UART RX ISR), tail only by the
 * consumer (main loop), so no critical section is needed on a single core.
 * Indices run freely and wrap naturally; capacity must be a power of two.
 */
#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *buf;
    size_t mask;            /**< capacity - 1 */
    volatile size_t head;   /**< write index (producer) */
    volatile size_t tail;   /**< read index (consumer) */
} ringbuf_t;

/** @return false if args are NULL or size is not a power of two. */
bool ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t size);

/** @return false if the buffer is full (byte is dropped). */
bool ringbuf_put(ringbuf_t *rb, uint8_t byte);

/** @return false if the buffer is empty. */
bool ringbuf_get(ringbuf_t *rb, uint8_t *byte);

/** @return number of bytes waiting to be read. */
size_t ringbuf_count(const ringbuf_t *rb);

#endif /* RINGBUF_H */
