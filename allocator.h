#include <pthread.h>
#include <stdbool.h>

#include "allocator_config.h"

typedef struct block_t {
    struct block_t *next;
} block_t;

typedef struct {
    uint8_t memory_pool[POOL_SIZE];
    block_t *free_list;
    bool block_allocated[NUM_BLOCKS];
    bool inited;
    pthread_mutex_t mutex; /* Depends on the system where it is used */
} BlockAllocator;

/* Do not use in interrupt */

void BlockAllocator_Init(BlockAllocator* allocator);
void* BlockAllocator_Allocate(BlockAllocator* allocator);
bool BlockAllocator_Free(BlockAllocator* allocator, void* block);
