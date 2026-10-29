#include "ringbuf.h"

bool ringbuf_init(ringbuf_t *rb, uint8_t *storage, size_t size)
{
    bool ok = (rb != NULL) && (storage != NULL) &&
              (size != 0U) && ((size & (size - 1U)) == 0U);

    if (ok) {
        rb->buf = storage;
        rb->mask = size - 1U;
        rb->head = 0U;
        rb->tail = 0U;
    }
    return ok;
}

size_t ringbuf_count(const ringbuf_t *rb)
{
    /* Unsigned subtraction stays correct when head wraps past SIZE_MAX. */
    return rb->head - rb->tail;
}

bool ringbuf_put(ringbuf_t *rb, uint8_t byte)
{
    bool ok = ringbuf_count(rb) <= rb->mask;

    if (ok) {
        rb->buf[rb->head & rb->mask] = byte;
        rb->head = rb->head + 1U;
    }
    return ok;
}

bool ringbuf_get(ringbuf_t *rb, uint8_t *byte)
{
    bool ok = ringbuf_count(rb) != 0U;

    if (ok) {
        *byte = rb->buf[rb->tail & rb->mask];
        rb->tail = rb->tail + 1U;
    }
    return ok;
}
