
#include "buddy_alloc.h"

int max_order_limit = 0;
unsigned long long max_memory_size = 0;
block_header *free_blocks_list[BUDDY_MAX_ORDER + 1];

int buddy_variable_init(int max_order, size_t memory_size, void *memory_start) {
    // this if should not be here, but it is for the sake of the test
    if (max_order > BUDDY_MAX_ORDER)
        return -1;
    
    max_order_limit = max_order;
    max_memory_size = memory_size;
    for (int i = 0; i < BUDDY_MAX_ORDER + 1; i++) {
        free_blocks_list[i] = NULL;
    }
    block_header *first_block = (block_header *)memory_start;
    first_block->size = memory_size - BUDDY_BLOCK_HEADER_SIZE;
    first_block->next = NULL;
    free_blocks_list[max_order] = first_block;
}

int my_log2(unsigned int size) {
  int i;
  for (i = 0; (1 << i) < size; i++);
  return i;
}

int order(unsigned int size) {
  return my_log2(size) - my_log2(BUDDY_MIN_BLOCK_SIZE);
}

/*
 * Calculates the buddy of the given block
 * A buddy is a block of equal size that is located next to the given block in memory.
 * Considering that the size of blocks are always powers of 2, 
 * the buddy can be located based on original block address and using bitwise XOR with the size of the block as the mask.
*/
block_header *buddy_of(block_header *block) {
    unsigned long size_mask = (unsigned long)(1 << my_log2(block->size + BUDDY_BLOCK_HEADER_SIZE));
    if (size_mask == max_memory_size)
        return NULL;
    
    return (block_header *)((unsigned long)(block) ^ size_mask);
}

void * mem_alloc_buddy(size_t size, char fill) {
    int target_order = order(size + BUDDY_BLOCK_HEADER_SIZE);
    for (int i = target_order; i <= max_order_limit; i++) {
        if (free_blocks_list[i] == NULL) {
            continue;
        }
        block_header *block = free_blocks_list[i];
        free_blocks_list[i] = free_blocks_list[i]->next;
        while (i > target_order) {
            i--;
            block_header *buddy = buddy_of(block);
            buddy->size = block->size / 2;
            buddy->next = free_blocks_list[i];
            free_blocks_list[i] = buddy;
        }
        block->size = size + BUDDY_BLOCK_HEADER_SIZE;
        return block + 1; // return the address after the header: block + sizeof(block_header)
    }
    if (target_order > max_order_limit) {
        return NULL;
    }
}