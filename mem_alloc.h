#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stddef.h>

#define FIRST_FIT 0
#define BUDDY_MEMORY 1
#define UNINITIALIZED -1

#define MetaDataSize 40

char error_message[100];

typedef struct MetaData {
    struct MetaData *next;
    struct MetaData *prev;
    char is_free;
    void *start;
    size_t size;
} MetaData;

void set_allocation_algorithm(int algorithm);

void set_maximum_size(size_t size);

void set_minimum_size(size_t size);

void *my_malloc(size_t size, char fill);

void my_free(void *block_ptr);

void *my_realloc(void *block_ptr, size_t size, char fill);

void show_stats();

#endif