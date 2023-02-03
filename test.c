 
#include <stdio.h>
#include "mem_alloc.h"

int main(int argc, char **argv) {
  int *data, *data2, *d1, *d2;

  data = (int *)my_malloc(100000 * sizeof(int), 'z');
  data2 = (int *)my_malloc(100000 * sizeof(int), 'z');
  for (int i = 0; i < 100000; i++) {
    data[i] = i;
    data2[i] = i;
  }
  for (int i = 0; i < 100000; i++) {
    printf("%p: %d\n", &data[i], data[i]);
  }
  for (int i = 0; i < 100000; i++) {
    printf("%lx: %d\n", (unsigned long) &data2[i], data2[i]);
  }
  my_free(data2);
  my_realloc(data, 150000, 'z');
  my_free(data);

  d1 = (int *)my_malloc(1000, 'z');
  d2 = (int *)my_malloc(1000, 'z');
  my_free(d1);
  my_free(d2);
  return 0;
}