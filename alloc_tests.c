#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "allocator.h"

bool test_allocate_normal() {
    BlockAllocator allocator;
    void *block = NULL;
    BlockAllocator_Init(&allocator);
    // Check init
    if (allocator.inited == false) {
        printf ("Fail: Cant init allocator\n");
        return false;
    }
    // Allocate all block
    for (uint32_t i = 0U; i < NUM_BLOCKS; i++) {
        block = BlockAllocator_Allocate(&allocator);
            if (block == NULL) {
                printf ("Fail: Cant allocate %u block, when NUM_BLOCKS=%u\n", i , NUM_BLOCKS);
                return false;
            };
    }
    // Try to allocate 1 more, must fail
    if (BlockAllocator_Allocate(&allocator) != NULL) {
        printf ("Fail: Allocate NUM_BLOCKS(%u) + 1\n", NUM_BLOCKS);
        return false;
    }
    // Free 1 block and try to allocate 1
    if (BlockAllocator_Free(&allocator, block) == false) {
        printf ("Fail: Cant free allocated block\n");
        return false;
    }
    if (BlockAllocator_Allocate(&allocator) == NULL) {
        printf ("Fail: Cant allocate block after free\n");
        return false;
    }
    return true;
}


bool test_normal_free() {
    BlockAllocator allocator;
    void *block[NUM_BLOCKS] = {NULL};

    // Allocate all block
    for (uint32_t i = 0U; i < NUM_BLOCKS; i++) {
        block[i] = BlockAllocator_Allocate(&allocator);
        if (block[i] == NULL) {
            printf ("Fail: Cant allocate %u block, when NUM_BLOCKS=%u\n", i , NUM_BLOCKS);
            return false;
        };
    }
    // Free all block
    for (uint32_t i = 0U; i < NUM_BLOCKS; i++) {
        if (BlockAllocator_Free(&allocator, block[i]) == false) {
            printf("Fail: Cant free %u block, when NUM_BLOCKS=%u\n", i, NUM_BLOCKS);
            return false;
        }
    }
    // Allocation and free of blocks one by one
    for (uint32_t i = 0U; i < 500U; i++) {
        block[0] = BlockAllocator_Allocate(&allocator);
        if (block[0] == NULL) {
            printf ("Fail: Cant allocate block, try %u\n", i);
            return false;
        };
        if (BlockAllocator_Free(&allocator, block[0]) == false) {
            printf("Fail: Cant free block, , try %u\n", i);
            return false;
        }
    }
    return true;
}

bool test_invalid_free() {
    BlockAllocator allocator;
    void *block = NULL;

    // Free without allocate
    if (BlockAllocator_Free(&allocator, block) == true) {
        printf ("Fail: Free without allocate\n");
        return false;
    }
    block = BlockAllocator_Allocate(&allocator);

    // Free invalid block
    if (BlockAllocator_Free(&allocator, (uint8_t*)block - 1u) == true) {
        printf ("Fail: Free invalid block\n");
        return false;
    }
    // Free without init
    allocator.inited = false;
    if (BlockAllocator_Free(&allocator, block) == true) {
        printf ("Fail: Cant free allocated block\n");
        return false;
    }
    // Normal case
    allocator.inited = true;
    if (BlockAllocator_Free(&allocator, block) == false) {
        printf ("Fail: Cant free normal block\n");
        return false;
    }
    // Double free
    if (BlockAllocator_Free(&allocator, block) == true) {
        printf ("Fail: Double free\n");
        return false;
    }
    return true;
}


int main() {

    if (test_allocate_normal() == true) {
        printf ("OK: Normal allocate\n");
    }

    if (test_normal_free() == true) {
        printf ("OK: Normal free\n");
    }

    if (test_invalid_free() == true) {
        printf ("OK: Invalid free\n");
    }

    return 0;
}