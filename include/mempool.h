/**
 * @file  mempool.h
 * @brief Fixed-size block allocator with O(1) alloc, no heap.
 *
 * Free blocks form a linked list whose "next" index is stored in the first
 * byte of each free block, so the pool needs no extra bookkeeping memory.
 * Storage must be aligned for the objects placed in it and block_size should
 * be a multiple of that alignment.
 */
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMPOOL_MAX_BLOCKS 254U  /**< 0xFF is reserved as end-of-list */

typedef struct {
    uint8_t *base;
    size_t block_size;
    uint8_t block_count;
    uint8_t free_head;      /**< index of first free block */
    uint8_t free_count;
} mempool_t;

/** @return false on NULL args, zero size, or block_count outside 1..MEMPOOL_MAX_BLOCKS. */
bool mempool_init(mempool_t *mp, uint8_t *storage, size_t block_size, uint8_t block_count);

/** @return pointer to a block, or NULL if the pool is exhausted. */
void *mempool_alloc(mempool_t *mp);

/**
 * Validates ptr against every block before releasing it (bounded O(n)),
 * so a stray pointer can never corrupt the free list.
 * @return false if ptr does not point to the start of a block in this pool.
 */
bool mempool_free(mempool_t *mp, const void *ptr);

#endif /* MEMPOOL_H */
