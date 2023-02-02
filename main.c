#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/resource.h>


int main(){
    // long tut = (long)sbrk(0);
    // printf("%ld\n%ld", tut, (long)sbrk(0));
    // printf("%lu %lu %lu\n", sizeof(int), sizeof(long int), sizeof(long long int));

    // int *p = (int *)malloc(10*sizeof(int));
    // printf("%ld - %ld\n", (long)*p,(long int)*p + 10*sizeof(int));

    // for (int i = 0; i < 10; i++)
    // {
    //     p[i] = i + 20;
    // }
    
    // long long start = (long long)*p;
    // long long end = (long long)sbrk(0);
    // printf("%lld - %lld\n", start,(long long int)*p + 10*sizeof(int));
    // printf("%lld\n%lld\n", (long long)sbrk(0), (long long)sbrk(0) - start);
    // long int *  temp = (long int *)malloc(10000*sizeof(long long int));
    // printf("%lld\n", (long long)sbrk(0));
    // temp[0] = 100;
    // temp[9999] = 999;
    // printf("-> %ld <-\n", *temp);
    // for (int i = 0; i < 10; i++)
    // {
    //     printf("%d\t", p[i]);
    // }
    printf("\n______________________________________\n");
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // printf("\n%ld\n", (long)sbrk(1000));
    // long cond = 0;
    // long int counter = 0;
    // printf("........\n");
    // while (cond != -1)
    // {
    //     counter++;
    //     cond = sbrk(10099999999);
    //     printf("%ld\n", cond);
    // }
    // printf("\n%ld\n", counter);
    // size_t base  = (unsigned long)sbrk(0);
    // printf("%lu\n", base);
    // int resource = RLIMIT_DATA;
    // struct rlimit limit;
    // int res = getrlimit(resource, &limit);
    // printf("get:%d\n", res);
    // printf("\nrlim_cur = %lu\n", limit.rlim_cur);
    // printf("\nrlim_max = %lu\n", limit.rlim_max);
    // limit.rlim_cur = 40960000;
    // limit.rlim_max = 40960000;

    // res = setrlimit(resource, &limit);
    // printf("set:%d\n", res);
    // getrlimit(resource, &limit);
    // printf("\nrlim_cur = %lu\n", limit.rlim_cur);
    // printf("\nrlim_max = %lu\n", limit.rlim_max);
    // printf("%ld\t..\t%ld\n", (long)sbrk(0) , (long)sbrk(0) + limit.rlim_cur);
    // printf("........\n");
    // long cond = 0;
    // long int counter = 0;
    // while (cond != -1)
    // {
    //     counter++;
    //     cond = (long)sbrk(4096);
    //     printf("%ld\n", cond);
    // }
    // printf("\n%ld\n", counter);
    // printf("%ld\n", sbrk(0));

    // printf("%ld\n", sbrk(-4096));
    // printf("back: %ld\n", sbrk(0));


    // printf("%ld\n", sbrk(1000));
    // printf("%ld\n", sbrk(1000));
    // printf("%ld\n", sbrk(1000));
    // printf("%ld\n", sbrk(1000));
    // printf("%ld\n", sbrk(10));

    // printf("check: %ld\n", sbrk(0));
    printf("%lu\n", (long)sbrk(0));
    unsigned long max_heap_size = 4096;
    void * heap_end = sbrk(max_heap_size);
    int counter = 0;
    while (heap_end != (void *)-1)
    {
        counter++;
        heap_end = sbrk(-max_heap_size);
        max_heap_size *= 2;
        heap_end = sbrk(max_heap_size);
        printf("%lu\t\t\t%lu\n", (long)sbrk(0), max_heap_size);
    }
    printf("\n\n%lu\t\t\t%lu\n", (long)sbrk(0), max_heap_size);
    sbrk(-max_heap_size);
    printf("%d\n", counter);
    printf("%lu\n", (unsigned long)sbrk(max_heap_size));
    printf("%lu\n", (unsigned long)sbrk(max_heap_size));
    printf("%lu\n", (unsigned long)sbrk(max_heap_size/2));
    printf("%lu\t\t\t%lu\n", (long)sbrk(0), max_heap_size);

    return 0;
}
