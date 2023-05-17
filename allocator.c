#include <stdint.h>

#include <stddef.h>

#include "allocator.h"


/* TODO: Instead of returning NULL or false, we can use an error handling mechanism,
 * such as returning error codes. */

void BlockAllocator_Init(BlockAllocator* allocator) {
    pthread_mutex_init(&allocator->mutex, NULL);

    /* TODO It is better to check it at the compilation stage, not runtime */
    if (DESIRED_BLOCK_SIZE > sizeof(block_t)) {
        allocator->free_list = (block_t *) allocator->memory_pool;
        block_t *current = allocator->free_list;

        for (uint32_t i = 1U; i < NUM_BLOCKS; ++i) {
            current->next = (block_t *) (allocator->memory_pool + (i * ALLOC_BLOCK_SIZE));
            current = current->next;
        }
        current->next = NULL;

        for (uint32_t i = 0U; i < NUM_BLOCKS; ++i) {
            allocator->block_allocated[i] = false;
        }
        allocator->inited = true;
    }
}

void* BlockAllocator_Allocate(BlockAllocator* allocator) {
    block_t *allocated_block = NULL;
    if (allocator != NULL) {
        if (allocator->inited == false) {
            BlockAllocator_Init(allocator);
        }
        pthread_mutex_lock(&allocator->mutex);

        /* Remove block from the free list if available */
        allocated_block = allocator->free_list;
        if (allocated_block != NULL) {
            allocator->free_list = allocated_block->next;
            /* Calculate the block index */
            ptrdiff_t offset = (uint8_t *) allocated_block - allocator->memory_pool;
            uint32_t block_index = (uint32_t) (offset / ALLOC_BLOCK_SIZE);
            /* Mark the block as allocated */
            allocator->block_allocated[block_index] = true;
        }

        pthread_mutex_unlock(&allocator->mutex);
    }
    /* Returns NULL if no blocks are available  */
    return allocated_block;
}

bool BlockAllocator_Free(BlockAllocator* allocator, void* block) {
    bool was_allocated = false;

    if ((block != NULL) && (allocator != NULL) && (allocator->inited == true)) {
        ptrdiff_t offset = (uint8_t*)block - allocator->memory_pool;
        /* Check if pointer is within the memory pool and pointer is aligned */
        if ((offset >= 0) && (offset < POOL_SIZE) && ((offset % ALLOC_BLOCK_SIZE) == 0U)) {
            /* Dangerous type reduction conversion (ptrdiff_t -> uint32_t).
             * If we think that blocks can be much better use size_t and 'UL' for literals */
            uint32_t block_index = (uint32_t) (offset / ALLOC_BLOCK_SIZE);
            (void) pthread_mutex_lock(&allocator->mutex);

            /* Check if the block is already free */
            was_allocated = allocator->block_allocated[block_index];
            if (was_allocated == true) {
                /* Mark the block as free */
                allocator->block_allocated[block_index] = false;
                /* Add the block to the free list */
                block_t* freed_block = (block_t*)block;
                freed_block->next = allocator->free_list;
                allocator->free_list = freed_block;
            }
            /* In some cases, it may be better to add freed memory zeroing */
            (void) pthread_mutex_unlock(&allocator->mutex);
        }
    }
    /* Returns false if the block was not previously allocated
     * or if invalid block/allocator is received
     * or allocator not inited*/
    return was_allocated;
}
