#include <unistd.h>
#include <pthread.h>
//////////////////////////////////
#include <stdio.h>
// [ ] TODO: Handle Thread Safety

int allocation_algorithm_set = 0;
int maximum_size_set = 0;
int minimum_size_set = 0;
int allocation_algorithm = -1;   // 0 = first fit, 1 = buddy memory
long long int maximum_size = 0;
long long int minimum_size = 0;

#def struct MetaData {
    struct MetaData *next;
    struct MetaData *prev;
    long long int size;
    int is_free;

}MetaData;

void set_allocation_algorithm(int algorithm) {
    if (allocation_algorithm_set == 0)
    {
        allocation_algorithm_set = 1;
        allocation_algorithm = algorithm;
    }
}

void set_maximum_size(long long int size) {
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

void set_minimum_size(long long int size) {
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

void *mem_alloc_first_fit(long long int size, char fill) {

}

void *my_malloc(long long int size, char fill) {
    allocation_algorithm_set = 1;
    if (allocation_algorithm == -1 || size <= 0)
    {
        printf("Memory allocation failed. Please set allocation algorithm and size > 0\n");
        return NULL;
    }
    if (maximum_size_set && size > maximum_size)
    {
        return NULL;
    }
    if (minimum_size_set && size < minimum_size)
    {
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
