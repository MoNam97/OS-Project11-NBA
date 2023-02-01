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

void * memory_start = NULL;
MetaData *allocated_head = NULL;
MetaData *allocated_tail = NULL;
MetaData *free_head = NULL;
MetaData *free_tail = NULL;

void first_fit_initialize(){
    memory_start = sbrk(0);
}

void set_allocation_algorithm(int algorithm) {
    if (allocation_algorithm_set == 0)
    {
        allocation_algorithm_set = 1;
        allocation_algorithm = algorithm;
        if (algorithm == FIRST_FIT)
            first_fit_initialize();
        
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
    if (free_head == NULL)
    {
        MetaData *new_block = (MetaData *)sbrk(size + MetaDataSize);
        if (new_block == -1)
        {
            strcpy(error_message, "sbrk failed.");
            return NULL;
        }
        new_block->next = NULL;
        new_block->prev = NULL;
        new_block->is_free = 0;
        new_block->start = new_block + MetaDataSize;
        new_block->size = size;
        
        allocated_head = new_block;
        return new_block->start;
    }
}

void *mem_alloc_buddy(size_t size, char fill) {

}

void *my_malloc(size_t size, char fill) {
    allocation_algorithm_set = 1;
    if (allocation_algorithm == -1 || size <= 0)
    {
        printf("Memory allocation failed. Please set allocation algorithm and size > 0\n");
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
        return mem_alloc_buddy(size,   fill);
    }
}


int main () {
    printf("%lu\n", sizeof(MetaData));
}