#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mem_alloc.h"
//////////////////////////////////
#include <stdio.h>
// [ ] TODO: Handle Thread Safety
char error_message[100];


int allocation_algorithm_set = 0;
int maximum_size_set = 0;
int minimum_size_set = 0;
int allocation_algorithm = UNINITIALIZED;   // 0 = first fit, 1 = buddy memory
size_t maximum_size = 0;
size_t minimum_size = 0;

void * heap_start = NULL;
size_t max_heap_size = 0;
MetaData *blocks_head = NULL;
// MetaData *allocated_tail = NULL;
// MetaData *free_head = NULL;
// MetaData *free_tail = NULL;

void first_fit_initialization(){
    heap_start = sbrk(0);
}

void buddy_memory_initialization(){
    heap_start = sbrk(0);

    max_heap_size = 32;
    void * heap_end = sbrk(max_heap_size);
    /* for test keep max_heap at 8192 */
    while (heap_end != -1 && max_heap_size < 8192)
    {
        heap_end = sbrk(-max_heap_size);
        max_heap_size *= 2;
        heap_end = sbrk(max_heap_size);
    }
    if (max_heap_size <= MetaDataSize){
        strcpy(error_message, "Not enough memory for heap");
        return;
    }
    
}

void set_allocation_algorithm(int algorithm) {
    if (allocation_algorithm_set == 0)
    {
        allocation_algorithm_set = 1;
        allocation_algorithm = algorithm;
        if (algorithm == FIRST_FIT)
            first_fit_initialization();
        else if (algorithm == BUDDY_MEMORY)
            buddy_memory_initialization();
        
    }
}

void set_maximum_size(size_t size) {
    // if (size < minimum_size)
    //     return;
    if (size < 0)
    {
        maximum_size_set = 0;  // reset
        return;
    }
    maximum_size_set = 1;
    maximum_size = size;
}

void set_minimum_size(size_t size) {
    // if (size > maximum_size)    
    //     return;
    if (size < 0)
    {
        minimum_size_set = 0;  // reset
        return;
    }
    minimum_size_set = 1;
    minimum_size = size;
}

void *mem_alloc_first_fit(size_t size, char fill) {
    if (blocks_head == NULL)
    {
        blocks_head = (MetaData *)sbrk(size + MetaDataSize);
        if (blocks_head == -1)
        {
            strcpy(error_message, "sbrk failed.");
            return NULL;
        }
        blocks_head->next = NULL;
        blocks_head->prev = NULL;
        blocks_head->is_free = 0;
        blocks_head->start = blocks_head + MetaDataSize;
        blocks_head->size = size;
        memset(blocks_head->start, fill, size);
        return blocks_head->start;
    }

    /*   If there is a free block with adequate size, allocate the new block to it.  */
    MetaData *last_block = NULL;
    MetaData *current = blocks_head;
    while (current != NULL) {
        if (current->is_free == 1 && current->size >= size)
            if (current->size - size > MetaDataSize)
            {
                MetaData *new_block = (MetaData *)(current->start + size);
                new_block->next = current->next;
                new_block->prev = current;
                if (current->next != NULL)
                    current->next->prev = new_block;
                current->next = new_block;

                new_block->is_free = 1;
                new_block->start = new_block + MetaDataSize;
                new_block->size = current->size - size - MetaDataSize;
                current->is_free = 0;
                current->size = size;
                memset(current->start, fill, size);
                return current->start;
            }
            else
            {
                // [ ] TODO: handle the no man's land portion of the size of the block in free function.
                current->size = size;
                current->is_free = 0;
                memset(current->start, fill, size);
                return current->start;
            }
        last_block = current;
        current = current->next;
    }

    /*   If there is no free block with adequate size, allocate a new block.  */
    size_t empty_space = sbrk(0) - last_block->start - last_block->size;
    MetaData *new_block = (MetaData *)sbrk(size + MetaDataSize - empty_space);
    if (new_block == -1)
    {
        strcpy(error_message, "sbrk failed. Heap can not be extended.");
        return NULL;
    }
    new_block -= empty_space;
    new_block->next = NULL;
    new_block->prev = last_block;
    last_block->next = new_block;
    new_block->is_free = 0;
    new_block->start = new_block + MetaDataSize;
    new_block->size = size;
    memset(new_block->start, fill, size);
    return new_block->start;
}

void *mem_alloc_buddy(size_t size, char fill) {

}

void *my_malloc(size_t size, char fill) {
    allocation_algorithm_set = 1;
    if (allocation_algorithm == -1 || size <= 0)
    {
        strcpy(error_message, "Memory allocation failed. Please set allocation algorithm and size > 0\n");
        return NULL;
    }
    if (maximum_size_set && size > maximum_size)
    {
        strcpy(error_message, "Requested size is larger than maximum size.");
        return NULL;
    }
    if (minimum_size_set && size < minimum_size)
    {
        strcpy(error_message, "Requested size is smaller than minimum size.");
        return NULL;
    }
    if (allocation_algorithm == 0)
    {
        return mem_alloc_first_fit(size, fill);
    }
    else
    {
        return mem_alloc_buddy(size, fill);
    }
}

int main(){
    
    printf("%d  =  %lu\n", MetaDataSize, sizeof(MetaData));
}