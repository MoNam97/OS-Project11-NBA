#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <stddef.h>

#define FIRST_FIT 0
#define BUDDY_MEMORY 1
#define UNINITIALIZED -1

#define MetaDataSize 40

typedef unsigned long long address_t;

extern char mem_alloc_error_message[];

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

///////////////////////// FIRST FIT /////////////////////////

#define BUDDY_MAX_ORDER 30
#define BUDDY_MIN_BLOCK_SIZE 32
#define BUDDY_MIN_ORDER 5

#define BUDDY_BLOCK_HEADER_SIZE 24

typedef struct block_header {
    char is_free;
    size_t size;
    struct block_header *next;
} block_header;

int buddy_variable_init(int max_order, size_t memory_size, void *memory_start);

void * mem_alloc_buddy(size_t size, char fill);

void free_buddy(void *block_ptr);

void show_buddy_memory();

///////////////////////// BUDDY MEMORY /////////////////////////

#endif