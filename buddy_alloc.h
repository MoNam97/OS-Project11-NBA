#ifndef BUDDY_ALLOC_H
#define BUDDY_ALLOC_H

#include <stddef.h>

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

#endif