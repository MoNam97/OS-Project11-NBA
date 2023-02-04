#include <stdio.h>

#include "mem_alloc.h"

int main(){
    printf("Hello World\n");
    int algorithm = 1;
    set_allocation_algorithm(algorithm);
    printf("Allocation algorithm: ");
    if (algorithm == 0){
        printf("First Fit\n");
    } else if (algorithm == 1){
        printf("Buddy Memory\n");
    } else {
        printf("Uninitialized\n");
    }

    char * t1 = (char *) my_malloc(100, 's');
    printf("t1: %p\n", t1);
    show_stats();

    char * t2 = (char *) my_malloc(64, 'z');
    printf("t2: %p\n", t2);
    show_stats();
    printf("The End\n");


    return 0;
}