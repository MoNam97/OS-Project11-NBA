#include <stddef.h>

#define FIRST_FIT 0
#define BEST_FIT 1
#define UNINITIALIZED -1

#define MetaDataSize 40

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

void *mem_alloc_first_fit(size_t size, char fill);