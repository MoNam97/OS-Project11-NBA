#include <unistd.h>
#include <string.h>
#include "buddy_alloc.h"


#include <stdio.h>
// this error message is not rgiht <<<<<<<<<<<<<<<<<<|||||||||
char error_message[100];

void * heap_start = NULL;
int max_order_limit = 0;
size_t max_memory_size = 0;


block_header *free_blocks_list[BUDDY_MAX_ORDER + 1];

int buddy_variable_init(int max_order, size_t memory_size, void *memory_start) {
    // this if should not be here, but it is for the sake of the test
    if (max_order > BUDDY_MAX_ORDER)
        return -1;
    
    max_order_limit = max_order;
    max_memory_size = memory_size;
    heap_start = memory_start;
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
 * Note1: This Logic work only if the heap is located at the beginning of the memory.
 * Note2: This function returns NULL if the block is the biggest block in the heap.
*/
block_header *buddy_of(block_header *block) {
    unsigned long size_mask = (unsigned long)(1 << my_log2(block->size + BUDDY_BLOCK_HEADER_SIZE));
    if (size_mask == max_memory_size)
        return NULL;
    
    block_header* relative_address =  (block_header *)(((unsigned long)block - (unsigned long)heap_start) ^ size_mask);
    return relative_address + (unsigned long)heap_start;
}


void * mem_alloc_buddy(size_t size, char fill) {
    int target_order = order(size + BUDDY_BLOCK_HEADER_SIZE);
    if (target_order > max_order_limit) {
        strcpy(error_message, "Requested size is too large");
        return NULL;
    }
    /*
     * find the first free block with adequate size
     * all indexes in the free_blocks_list array are ordered from the smallest to the largest
     * so we start from the target_order and go up until we find a free block
     * all indexes before found one are guaranteed to be empty.
    */
    for (int i = target_order; i <= max_order_limit; i++) {
        if (free_blocks_list[i] == NULL) {
            continue;
        }
        block_header *block = free_blocks_list[i];
        free_blocks_list[i] = free_blocks_list[i]->next;
        while (i > target_order) {
            i--;
            block->size = (block->size + BUDDY_BLOCK_HEADER_SIZE) / 2 - BUDDY_BLOCK_HEADER_SIZE;
            block_header *buddy = buddy_of(block);
            buddy->size = block->size;
            buddy->next = NULL;
            free_blocks_list[i] = buddy;
        }
        block->size = size;
        block->is_free = 0;
        memset(block + BUDDY_BLOCK_HEADER_SIZE, fill, size);
        return block + BUDDY_BLOCK_HEADER_SIZE;
    }

    strcpy(error_message, "No free blocks with adequate size available.");
    return NULL;
}


void add_to_free_list(block_header * block, int order) {
    block_header * prev = NULL;
    block_header * current = free_blocks_list[order];
    while (current != NULL)
    {
        if (current > block)
        {
            block->next = current;
            if (prev == NULL)
                free_blocks_list[order] = block;
            else
                prev->next = block;
            return;
        }
        prev = current;
        current = current->next;
    }
    if (prev == NULL)
        free_blocks_list[order] = block;
    else
        prev->next = block;
    block->next = NULL;   
}

void * get_buddy_block(void *block_ptr){
    if (block_ptr == NULL){
        return NULL;
    }
    if (block_ptr < ((void *)free_blocks_list[max_order_limit] + BUDDY_BLOCK_HEADER_SIZE) || block_ptr > sbrk(0)){
        return NULL;
    }
    block_header * block = NULL;
    block = (block_header *)(block_ptr - (void *)BUDDY_BLOCK_HEADER_SIZE);
    return block;
}

void merge_buddy_blocks(block_header *block){
    block_header *buddy_block = buddy_of(block);
    if (buddy_block != NULL && buddy_block->is_free == 1){
        if (buddy_block < block){
            buddy_block->next = block->next;
            buddy_block->size = (buddy_block->size >> 1);
        }
        else {
            block->next = buddy_block->next;
            block->size = (block->size >> 1);
        }
    }
}

void free_buddy(void *block_ptr){
    if (block_ptr == NULL){
        return;
    }
    block_header *block = get_buddy_block(block_ptr);
    if (block == NULL){
        return;
    }
    block->is_free = 1;
    block->size = (1 << (order(block->size) + 1));
    merge_buddy_blocks(block);
}

void show_buddy_memory() {
    block_header *block = (block_header *)heap_start;
    size_t allocate_size = 0;
    size_t free_size = 0;

    printf("Allocated blocks:\n");
    printf("start_add\tend_add\tsize\n");
    while (block < (block_header *)((size_t)heap_start + max_memory_size))
    {
        if (block->is_free == 0){
            printf("%p\t%p\t%ld\n", block + BUDDY_BLOCK_HEADER_SIZE, block + BUDDY_BLOCK_HEADER_SIZE + block->size -1 , block->size);
            allocate_size += block->size;
        }
        allocate_size += BUDDY_BLOCK_HEADER_SIZE;
        block = (block_header *)((size_t)block + block->size + BUDDY_BLOCK_HEADER_SIZE);
    }

    block = (block_header *)heap_start;
    printf("\nFree blocks:\n");
    printf("start_add\tend_add\tsize\n");
    while (block < (block_header *)((size_t)heap_start + max_memory_size))
    {
        if (block->is_free == 1){
            printf("%p\t%p\t%ld\n", block + BUDDY_BLOCK_HEADER_SIZE, block + BUDDY_BLOCK_HEADER_SIZE + block->size -1 , block->size);
            // Don't know what to change %d to in order to fix warning
            free_size += block->size;
        }
        block = (block_header *)((size_t)block + block->size + BUDDY_BLOCK_HEADER_SIZE);
    }

    printf("\nAllocated size = %lu\n", allocate_size);
    printf("Free size = %lu\n", free_size);
    printf("sbrk minus the space devoted to blocks = %p\n", sbrk(0) - allocate_size - free_size);
}