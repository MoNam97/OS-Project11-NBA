#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mem_alloc.h"

#include <stdio.h>
// [ ] TODO: Handle Thread Safety

char mem_alloc_error_message[100];

int allocation_algorithm_set = 0;
int maximum_size_set = 0;
int minimum_size_set = 0;
int allocation_algorithm = UNINITIALIZED;   // 0 = first fit, 1 = buddy memory
size_t maximum_size = 0;
size_t minimum_size = 0;

void * heap_start = NULL;

int max_order_limit = 0;
size_t max_memory_size = 0;

MetaData *blocks_head = NULL;
block_header *free_blocks_list[BUDDY_MAX_ORDER + 1];


int buddy_variable_init(int max_order, size_t memory_size, void *memory_start);

void * mem_alloc_buddy(size_t size, char fill);

void free_buddy(void *block_ptr);

void show_buddy_memory();

void first_fit_initialization(){
    heap_start = sbrk(0);
}

void buddy_memory_initialization(){
    heap_start = sbrk(0);

    size_t max_heap_size = BUDDY_MIN_BLOCK_SIZE;
    void * heap_end = sbrk(max_heap_size);
    /* for test keep max_order at 8 should be BUDDY_MAX_ORDER */
    int order = 0;
    while (heap_end != (void *)-1 && order < 9)
    {
        order++;
        heap_end = sbrk(-max_heap_size);
        max_heap_size *= 2;
        heap_end = sbrk(max_heap_size);
    }
    max_heap_size /= 2;
    order--;
    if (order < 0){
        strcpy(mem_alloc_error_message, "Not enough memory for heap");
        return;
    }
    buddy_variable_init(order, max_heap_size, heap_start);
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
    
    /*   initializing the first block of memory   */
    if (blocks_head == NULL)
    {
        
        blocks_head = (MetaData *)sbrk(size + MetaDataSize);
        if (blocks_head == (void *)-1)
        {
            strcpy(mem_alloc_error_message, "sbrk failed.");
            return NULL;
        }
        blocks_head->next = NULL;
        blocks_head->prev = NULL;
        blocks_head->is_free = 0;
        blocks_head->start = (void *)((size_t)blocks_head + MetaDataSize);
        blocks_head->size = size;
        memset(blocks_head->start, fill, size);
        // printf("sbrk: %lu\tblocks_head: %lu\t blocks_head->start: %lu\t blocks_head->size: %lu\t delta:%d\n",
        //  (address_t)sbrk(0), blocks_head, blocks_head->start, blocks_head->size, blocks_head->start - (address_t)blocks_head);
        return blocks_head->start;
    }

    /*   If there is a free block with adequate size, allocate the new block to it.  */
    MetaData *last_block = NULL;
    MetaData *current = blocks_head;
    while (current != NULL) {
        
        if (current->is_free == 1 && current->size >= size) {
            
            if (current->size - size > MetaDataSize)
            {
                MetaData *new_block = (MetaData *)(current->start + size);
                new_block->next = current->next;
                new_block->prev = current;
                if (current->next != NULL)
                    current->next->prev = new_block;
                current->next = new_block;

                new_block->is_free = 1;
                new_block->start = (void *)((address_t)new_block + MetaDataSize);
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
        }
        last_block = current;
        current = current->next;
    }
    /*   If there is no free block with adequate size, allocate a new block.  */
    size_t empty_space = (size_t)((address_t)sbrk(0) - (address_t)last_block->start) - last_block->size;
    MetaData *new_block = (MetaData *)sbrk(size + MetaDataSize - empty_space);

    /////////////////////////////   For Test   /////////////////////////////
    // if ((address_t)sbrk(0) - (address_t) heap_start > 900)
    // {
        
    //     sbrk(-(size + MetaDataSize - empty_space));
        
    //     strcpy(mem_alloc_error_message, " Heap Limit for Test Exceeded.");
        
    //     return NULL;
    // }
    ///////////////////////////////////////////////////////////////////////
    if (new_block == (void *)-1)
    {
        strcpy(mem_alloc_error_message, "sbrk failed. Heap can not be extended.");
        return NULL;
    }
    new_block -= empty_space;
    new_block->next = NULL;
    new_block->prev = last_block;
    last_block->next = new_block;
    new_block->is_free = 0;
    new_block->start = (void *)((address_t)new_block + MetaDataSize);
    new_block->size = size;
    memset(new_block->start, fill, size);
    return new_block->start;
}



void *my_malloc(size_t size, char fill) {
    allocation_algorithm_set = 1;
    if (allocation_algorithm == -1 || size <= 0)
    {
        strcpy(mem_alloc_error_message, "Memory allocation failed. Please set allocation algorithm and size > 0\n");
        return NULL;
    }
    if (maximum_size_set && size > maximum_size)
    {
        strcpy(mem_alloc_error_message, "Requested size is larger than maximum size.");
        return NULL;
    }
    if (minimum_size_set && size < minimum_size)
    {
        strcpy(mem_alloc_error_message, "Requested size is smaller than minimum size.");
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
    block = (MetaData *)((address_t)block_ptr - MetaDataSize);
    if (block->is_free == 1)
        return NULL;
    return block;
}

void merge_blocks(MetaData * curr_block){
    MetaData * prev_block = curr_block->prev;
    MetaData * next_block = curr_block->next;

    if (prev_block != NULL && prev_block->is_free == 1) {
        prev_block->next = curr_block->next;
        if (next_block != NULL) {
            next_block->prev = prev_block;
        }
        prev_block->size = ((void *)curr_block + MetaDataSize) - ((void *)prev_block + MetaDataSize) + curr_block->size;

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
        curr_block->is_free = 1;
        curr_block->size = ((void *)next_block + MetaDataSize) - ((void *)curr_block + MetaDataSize) + next_block->size;
        curr_block->next = next_block->next;
        next_block = next_block->next;
        if (next_block != NULL) {
            next_block->prev = curr_block;
        }
        return;
    }
}

void free_first_fit(void *block_ptr){
    // if (block_ptr == NULL){
    //     return;
    // }    get_block handles checking for NULL
    MetaData * block = get_block(block_ptr);
    if (block == NULL){
        return;
    }
    block->is_free = 1;
    merge_blocks(block);
}

void my_free(void *block){
    if (allocation_algorithm) {
        free_buddy(block);
    }
    else {
        free_first_fit(block);
    }
}

size_t min(size_t a, size_t b){
    if (a < b){
        return a;
    }
    return b;
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
  
    memcpy(buffer, block->start, init_size);

    my_free(block_ptr);
  
    char * new_start = (char *)my_malloc(size, fill);
    memcpy(new_start, block->start, min(init_size, size));
    return (void *)new_start;
}


void show_first_fit() {
    MetaData * block = blocks_head;
    size_t allocate_size = 0;
    size_t free_size = 0;
    
    printf("Allocated blocks:\n");
    while(block != NULL) {
        if(!block->is_free) {
            printf("%p\t%p\t%lu\n", block->start, block->start + block->size, block->size);
            allocate_size += block->size;
        }
        allocate_size +=  MetaDataSize;
        block = block->next;
    }
    
    printf("\nFree blocks:\n");
    block = blocks_head;
    while(block != NULL) {
        if (block->is_free) {
            printf("%p\t%p\t%lu\n", block->start, block->start + block->size, block->size);
            free_size += block->size;
        }
        block = block->next;
    }

    printf("\nAllocated size = %lu\n", allocate_size);
    printf("Free size = %lu\n", free_size);
    printf("sbrk minus the space devoted to blocks = %p\n", sbrk(0) - allocate_size - free_size);
}

void show_stats() {
    if (allocation_algorithm_set == 0)
    {
        strcpy(mem_alloc_error_message, "Please set allocation algorithm first.");
        return;
    }
    if(allocation_algorithm)
        show_buddy_memory();
    else
        show_first_fit();
    printf("\n\n");
}

////////////////////////// Buddy System //////////////////////////


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
    first_block->is_free = 1;
    first_block->size = memory_size - BUDDY_BLOCK_HEADER_SIZE;
    first_block->next = NULL;
    free_blocks_list[max_order] = first_block;
}

int my_log2(size_t size) {
  int i;
  for (i = 0; (1 << i) < size; i++);
  return i;
}

int order(size_t size) {
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
    
    block_header* relative_address =  (block_header *)(((address_t)block - (address_t)heap_start) ^ size_mask);
    return (block_header *)((address_t)relative_address + (address_t)heap_start);
}


void * mem_alloc_buddy(size_t size, char fill) {
    int target_order = order(size + BUDDY_BLOCK_HEADER_SIZE);
    if (target_order > max_order_limit) {
        strcpy(mem_alloc_error_message, "Requested size is too large");
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
            buddy->is_free = 1;
            buddy->next = NULL;
            free_blocks_list[i] = buddy;
        }
        block->size = size;
        block->is_free = 0;
        memset((void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE), fill, size);
        return (void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE);
    }

    strcpy(mem_alloc_error_message, "No free blocks with adequate size available.");
    return NULL;
}


void add_to_free_list(block_header * block, int b_order) {
    block_header * prev = NULL;
    block_header * current = free_blocks_list[b_order];
    while (current != NULL)
    {
        if (current > block)
        {
            block->next = current;
            if (prev == NULL)
                free_blocks_list[b_order] = block;
            else
                prev->next = block;
            return;
        }
        prev = current;
        current = current->next;
    }
    if (prev == NULL)
        free_blocks_list[b_order] = block;
    else
        prev->next = block;
    block->next = NULL;   
}

void remove_from_free_list(block_header * block, int b_order) {
    block_header * prev = NULL;
    block_header * current = free_blocks_list[b_order];
    while (current != NULL)
    {
        if (current == block)
        {
            if (prev == NULL)
                free_blocks_list[b_order] = current->next;
            else
                prev->next = current->next;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void * get_buddy_block(void *block_ptr){
    if (block_ptr == NULL || block_ptr < heap_start || block_ptr >= sbrk(0)){
        return NULL;
    }
    block_header * block = NULL;
    block = (block_header *)((address_t)block_ptr - BUDDY_BLOCK_HEADER_SIZE);

    if (block->is_free == 1)
        return NULL;
    return block;
}

void merge_buddy_blocks(block_header *block, int b_order){
    block_header *buddy_block = buddy_of(block);
    if (buddy_block != NULL && buddy_block->is_free == 1){
        if (buddy_block < block){
            buddy_block->next = NULL;
            buddy_block->size = (1 << (b_order + BUDDY_MIN_ORDER + 1)) - BUDDY_BLOCK_HEADER_SIZE;
            remove_from_free_list(buddy_block, b_order);
            merge_buddy_blocks(buddy_block, b_order + 1);
        }
        else {
            block->next = NULL;
            block->size = (1 << (b_order + BUDDY_MIN_ORDER + 1)) - BUDDY_BLOCK_HEADER_SIZE;
            remove_from_free_list(buddy_block, b_order);
            merge_buddy_blocks(block, b_order + 1);
        }
    }
    else {
        add_to_free_list(block, b_order);
    }
}

void free_buddy(void *block_ptr){
    block_header *block = get_buddy_block(block_ptr);
    if (block == NULL){
        return;
    }
    block->is_free = 1;
    int block_order = order(block->size + BUDDY_BLOCK_HEADER_SIZE);
    block->size = (1 << block_order + BUDDY_MIN_ORDER) - BUDDY_BLOCK_HEADER_SIZE; 
    merge_buddy_blocks(block, block_order);
}

void show_buddy_memory() {
    block_header *block = (block_header *)heap_start;
    size_t allocate_size = 0;
    size_t free_size = 0;

    printf("Allocated blocks:\n");
    printf("  start_add        end_add      size\n");
    while (block < (block_header *)((size_t)heap_start + max_memory_size))
    {
        if (block->is_free == 0){
            printf("%p\t%p\t%lu\n", (void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE),
             (void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE + block->size - 1) , block->size);
            allocate_size += block->size;
        }
        allocate_size += BUDDY_BLOCK_HEADER_SIZE;
        size_t block_size = 1 << my_log2(block->size + BUDDY_BLOCK_HEADER_SIZE);
        block = (block_header *)((address_t)block + block_size);
    }

    block = (block_header *)heap_start;
    printf("\nFree blocks:\n");
    printf("  start_add        end_add      size\n");
    while (block < (block_header *)((address_t)heap_start + max_memory_size))
    {
        if (block->is_free == 1){
            printf("%p\t%p\t%lu\n", (void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE),
             (void *)((address_t)block + BUDDY_BLOCK_HEADER_SIZE + block->size - 1) , block->size);
            free_size += block->size;
        }
        size_t block_size = 1 << my_log2(block->size + BUDDY_BLOCK_HEADER_SIZE);
        block = (block_header *)((address_t)block + block_size);
    }

    printf("\nAllocated size = %lu\n", allocate_size);
    printf("Free size = %lu\n", free_size);
    printf("sbrk minus the space devoted to blocks = %p\n", (void *)((address_t)sbrk(0) - allocate_size - free_size));
}