#include <string.h>
#include <stdio.h>
#include "mem_alloc.h"

int main(int argc, char **argv) {
  int algorithm = FIRST_FIT; // causes segmentation fault
  int MAX_SIZE = 10000, MIN_SIZE = 20;
  //int algorithm = BUDDY_MEMORY;

  set_allocation_algorithm(algorithm);
  set_maximum_size(MAX_SIZE);
  set_minimum_size(MIN_SIZE);

  char *dummy = my_malloc(10, 'z');
  printf("Testing allocation less than minimum size: %s\n", dummy);

  dummy = my_malloc(20000, 'z');
  printf("Testing allocation more than maximum size: %s\n", dummy);

  char *str = my_malloc(100, '8');
  str[99] = '\0';
  printf("%s\n\n", str);

  char * hello = "Hello world\n";
  memcpy(str, hello, strlen(hello));
  printf("%s\n\n", str);

  my_free(NULL);
  my_free(str);
  printf("%s\n\n", str);

  size_t size = 20;
  int *arr;
  arr = my_malloc(size * sizeof(int), 'z');
  printf("\n");
  for(int i = 0; i < size; i++) 
    arr[i] = i;
  for(int i = 0; i < size; i++) 
    printf("%p:\t%d\n", &arr[i], i);
  
  size *= 2;
  arr = my_realloc(arr, size * sizeof(int), 'z');
  printf("\n");
  
  for(int i = 0; i < size; i++) 
    arr[i] = i;
  
  for(int i = 0; i < size; i++) 
    printf("%p:\t%d\n", &arr[i], i);
  
  
  arr = my_realloc(arr, 0, 'z');
  printf("%p\n", arr);
  
  return 0;
}