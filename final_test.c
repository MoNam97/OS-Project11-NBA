#include <stdio.h>

#include "mem_alloc.h"

void check_error(void *ptr){
    if (ptr == NULL)
        printf("Error: %s\n", mem_alloc_error_message);  
}

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




    } else if (algorithm == 1){
        printf("//////////////////////////////////////////   Buddy System   ///////////////////////////////////////////\n");
        show_stats();
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

        show_stats();

        my_free(char_ptr[1]);
        show_stats();


        my_free(char_ptr[2]);
        show_stats();
        my_free(char_ptr[3]);
        show_stats();
        my_free(char_ptr[0]);
        show_stats();

        char_ptr[8] = (char *) my_realloc(char_ptr[8], 400, 'Z');

        show_stats();

        char * t1 = (char *) my_malloc(2000, 'W');

        show_stats();

        for (int i = 4; i < 10; i++)
            my_free(char_ptr[i]);
        show_stats();
        my_free(t1);
        show_stats();
    }
    printf("The End\n");

    return 0;
}

