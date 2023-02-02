#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mem_alloc.h"
#include "buddy_alloc.h"
//////////////////////////////////
#include <stdio.h>
// [ ] TODO: Handle Thread Safety



int allocation_algorithm_set = 0;
int maximum_size_set = 0;
int minimum_size_set = 0;
int allocation_algorithm = UNINITIALIZED;   // 0 = first fit, 1 = buddy memory
size_t maximum_size = 0;
size_t minimum_size = 0;

void * heap_start = NULL;
MetaData *blocks_head = NULL;


void first_fit_initialization(){
    heap_start = sbrk(0);
}

void buddy_memory_initialization(){
    heap_start = sbrk(0);

    size_t max_heap_size = BUDDY_MIN_BLOCK_SIZE;
    void * heap_end = sbrk(max_heap_size);
    /* for test keep max_order at 8 should be BUDDY_MAX_ORDER */
    int order = 0;
    while (heap_end != -1 && order < 8)
    {
        order++;
        heap_end = sbrk(-max_heap_size);
        max_heap_size *= 2;
        heap_end = sbrk(max_heap_size);
    }
    max_heap_size /= 2;
    order--;
    if (order < 0){
        strcpy(error_message, "Not enough memory for heap");
        return;
    }
    buddy_var_init(order, max_heap_size, heap_start);
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

MetaData * get_block(void* block_ptr){
    if (block_ptr == NULL){
        return NULL;
    }
    if (block_ptr < ((void*)blocks_head+MetaDataSize) || block_ptr > sbrk(0)) {
        return NULL;
    }
    MetaData * block = NULL;
    block = (MetaData *)(block_ptr - (void*)MetaDataSize);
    return block;
}

void my_free(void *block_ptr){
    if (block_ptr == NULL){
        return;
    }
    MetaData * block = get_block(block_ptr);
    if (block == NULL){
        return;
    }
    block->is_free = 1;
    merge_blocks(block);
}

void merge_blocks(MetaData * block){
    MetaData * prev_block = block->prev;
    MetaData * next_block = block->next;

    if (prev_block != NULL && prev_block->is_free == 1) {
        prev_block->next = block->next;
        if (next_block != NULL) {
            next_block->prev = prev_block;
        }
        prev_block->size = ((void *)block + MetaDataSize) - ((void *)prev_block + MetaDataSize) + block->size;

        if (next_block != NULL && next_block->is_free == 1) {
            prev_block->next = next_block->next;
            if (next_block->next != NULL) {
                next_block->next->prev = prev_block;
            }
            prev_block->size = ((void *)next_block + MetaDataSize) - ((void *)prev_block + MetaDataSize) + next_block->size;
        }
        return;
    }
    else if (next_block != NULL && next_block->is_free == 1) {
        block->is_free = 1;
        block->size = ((void *)next_block + MetaDataSize) - ((void *)block + MetaDataSize) + next_block->size;
        block->next = next_block->next;
        next_block = next_block->next;
        if (next_block != NULL) {
            next_block->prev = block;
        }
        return;
    }
}

void *my_realloc(void *block_ptr, size_t size, char fill) {
    if (size <= 0) {
        my_free(block_ptr);
        return NULL;
    }
    if (block_ptr == NULL) {
        return my_malloc(size, fill);
    }
    MetaData * block = get_block(block_ptr);
    size_t init_size = block->size;
    char buffer[block->size];

    memcpy(buffer, block->size, init_size);

    my_free(block_ptr);

    void * new_start = my_malloc(size, fill);
    memcpy(new_start, buffer, MIN(init_size, size));
    return new_start;
}

void show_buddy_memory() {

}

void show_first_fit() {
    MetaData * block = blocks_head;
    printf("Allocated blocks:\n");
    while(block != NULL) {
        if(!block->is_free) {
            printf("%d\t%d\t%d\n", block->start, block->start + block->size, block->size);
        }
        block = get_block(block->next);
    }
    printf("\nFree blocks:\n");
    while(block != NULL) {
        if(block->is_free) {
            printf("%d\t%d\t%d\n", block->start, block->start + block->size, block->size);
        }
        block = get_block(block->next);
    }
}

void show_stats() {
    if(allocation_algorithm)
        show_buddy_memory();
    else
        show_first_fit();
}


int main(){
    
    printf("%d  =  %lu\n", MetaDataSize, sizeof(MetaData));
}