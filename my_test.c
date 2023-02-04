#include <string.h>
#include <stdio.h>
#include "mem_alloc.h"

int main(int argc, char **argv) {
  int algorithm = FIRST_FIT;
  int MAX_SIZE = 10000, MIN_SIZE = 20;
  //int algorithm = BUDDY_MEMORY;

  set_allocation_algorithm(algorithm);
  set_maximum_size(MAX_SIZE);
  set_minimum_size(MIN_SIZE);

  set_maximum_size(4 * MAX_SIZE);
  set_minimum_size(MIN_SIZE / 4);

  char *dummy = my_malloc(10, 'z');
  prinf("%Testing allocation less than minimum size: %s\n", dummy);

  dummy = my_malloc(20000, 'z');
  prinf("%Testing allocation more than maximum size: %s\n", dummy);

  char *str = my_malloc(100, '8');
  str[99] = '\0';
  printf("%s\n\n", str);

  char * hello = "Hello world\n";
  memcpy(str, hello, strlen(hello));
  printf("%s\n", str);

  my_free(NULL);
  my_free(str);
  printf("%s\n", str);

  size_t size = 20
  arr = my_malloc(size * sizeof(int), 'z');
  printf("\n");
  for(int i = 0; i < size; i++) 
    arr[i] = i;
  for(int i = 0; i < size; i++) 
    print("%p:\t%d\n", &arr[i], i);
  
  size *= 2;
  arr = my_realloc(arr, size * sizeof(int), 'z');
  printf("\n");
  for(int i = 0; i < size; i++) 
    arr[i] = i;
  for(int i = 0; i < size; i++) 
    print("%p:\t%d\n", &arr[i], i);

  arr = my_realloc(arr, 0, 'z');
  printf("%s", arr);

  arr = my_realloc(arr, 20, 'z');
  arr[18] = '\n';
  arr[19] = '\0';
  printf("%s", arr);
  
  return 0;
}