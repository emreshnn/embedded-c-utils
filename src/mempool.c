#include "mempool.h"

#define END_OF_LIST 0xFFU

bool mempool_init(mempool_t *mp, uint8_t *storage, size_t block_size, uint8_t block_count)
{
    bool ok = (mp != NULL) && (storage != NULL) &&
              (block_size != 0U) && (block_count != 0U) &&
              (block_count <= MEMPOOL_MAX_BLOCKS);

    if (ok) {
        mp->base = storage;
        mp->block_size = block_size;
        mp->block_count = block_count;
        mp->free_head = 0U;
        mp->free_count = block_count;

        /* Chain every block to the next one; last block ends the list. */
        for (uint8_t i = 0U; i < block_count; i++) {
            uint8_t next = (i == (block_count - 1U)) ? END_OF_LIST : (uint8_t)(i + 1U);
            storage[(size_t)i * block_size] = next;
        }
    }
    return ok;
}

void *mempool_alloc(mempool_t *mp)
{
    void *block = NULL;

    if (mp->free_head != END_OF_LIST) {
        uint8_t *p = &mp->base[(size_t)mp->free_head * mp->block_size];
        mp->free_head = *p;
        mp->free_count--;
        block = p;
    }
    return block;
}

bool mempool_free(mempool_t *mp, const void *ptr)
{
    bool ok = false;

    /* Only equality is used: relational compares of unrelated pointers are UB. */
    for (uint8_t i = 0U; (i < mp->block_count) && !ok; i++) {
        uint8_t *block = &mp->base[(size_t)i * mp->block_size];

        if (ptr == block) {
            *block = mp->free_head;
            mp->free_head = i;
            mp->free_count++;
            ok = true;
        }
    }
    return ok;
}
