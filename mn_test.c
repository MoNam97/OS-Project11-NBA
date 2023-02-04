#include <stdio.h>

#include "mem_alloc.h"

void check_error(void *ptr){
    if (ptr == NULL)
        printf("Error: %s\n", mem_alloc_error_message);  
}

int main(){

    printf("Hello World\n");
    int algorithm = 0;
    set_allocation_algorithm(algorithm);
    printf("Allocation algorithm: ");
    if (algorithm == 0){
        printf("First Fit\n");
    } else if (algorithm == 1){
        printf("Buddy Memory\n");
    } else {
        printf("Uninitialized\n");
    }

    // char * t1 = (char *) my_malloc(1000, 's');
    // printf("t1: %p\n", t1);
    // show_stats();

    // char * t2 = (char *) my_malloc(64, 'z');
    // printf("t2: %p\n", t2);
    // check_error(t2);
    // show_stats();

    // char * t3 = (char *) my_malloc(520, 'a');
    // printf("t3: %p\n", t3);
    // check_error(t3);
    // show_stats();

    // char * t4 = (char *) my_malloc(76031, 'b');
    // printf("t4: %p\n", t4);
    // check_error(t4);
    // show_stats();

    if (algorithm == 0){
        
        printf("//////////////////////////////////////////   First Fit   ///////////////////////////////////////////\n");
        char * char_ptr[10];
        int k = 0;
        while (k < 10){
            char_ptr[k] = (char *) my_malloc(100, (k%30) + 'a');
            printf("char_ptr[%d]: %p\n", k, char_ptr[k]);
            check_error(char_ptr[k]);
            show_stats();
            if (char_ptr[k] == NULL)
                break;
            k++;
        }
        printf("/////////////////////////////////////////////////////////////////////////////////////\n");
        for (int i = 0; i < k; i++)
        {
            printf("char_ptr[%d]: %p\n", i, char_ptr[i]);
            for (int j = 0; j < 100; j++)
            {
                printf("%c", char_ptr[i][j]);
            }
            printf("\n");
        }
        printf("/////////////////////////////////////////////////////////////////////////////////////\n");

        my_free(char_ptr[2]);
        show_stats();
        my_free(char_ptr[4]);
        show_stats();
        my_free(char_ptr[3]);
        show_stats();
        
        char_ptr[0] = (char *) my_realloc(char_ptr[0], 200, 'z');
        show_stats();
        char_ptr[1] = (char *) my_realloc(char_ptr[1], 200, 'z');
        show_stats();
        printf("The End\n");
    } else if (algorithm == 1){
        printf("//////////////////////////////////////////   Buddy System   ///////////////////////////////////////////\n");

        int max_order = 30;
        size_t memory_size = 2000000000;
        void *memory_start = malloc(memory_size);
        buddy_variable_init(max_order, memory_size, memory_start);
        show_buddy_memory();
        char *t1 = (char *) mem_alloc_buddy(100, 's');
        printf("t1: %p\n", t1);
        show_buddy_memory();
        char *t2 = (char *) mem_alloc_buddy(64, 'z');
        printf("t2: %p\n", t2);
        show_buddy_memory();
        char *t3 = (char *) mem_alloc_buddy(520, 'a');
        printf("t3: %p\n", t3);
        show_buddy_memory();
        char *t4 = (char *) mem_alloc_buddy(76031, 'b');
        printf("t4: %p\n", t4);
        show_buddy_memory();
        free_buddy(t1);
        show_buddy_memory();
        free_buddy(t2);
        show_buddy_memory();
        free_buddy(t3);
        show_buddy_memory();
        free_buddy(t4);
        show_buddy_memory();
    }
    return 0;
}

