#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#include <sys/resource.h>


int main(){
    long tut = sbrk(0);
    printf("%ld\n%ld", tut, (long)sbrk(0));
    printf("%lu %lu %lu\n", sizeof(int), sizeof(long int), sizeof(long long int));

    int *p = (int *)malloc(10*sizeof(int));
    printf("%ld - %ld\n", (long)*p,(long int)*p + 10*sizeof(int));

    for (int i = 0; i < 10; i++)
    {
        p[i] = i + 20;
    }
    
    long long start = (long long)*p;
    long long end = (long long)sbrk(0);
    printf("%lld - %lld\n", start,(long long int)*p + 10*sizeof(int));
    printf("%lld\n%lld\n", (long long)sbrk(0), (long long)sbrk(0) - start);
    long int *  temp = (long long int *)malloc(10000*sizeof(long long int));
    printf("%lld\n", (long long)sbrk(0));
    temp[0] = 100;
    temp[9999] = 999;
    printf("-> %ld <-\n", *temp);
    for (int i = 0; i < 10; i++)
    {
        printf("%d\t", p[i]);
    }
    printf("\n______________________________________");
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
    int resource = RLIMIT_DATA;
    struct rlimit limit;
    int res = getrlimit(resource, &limit);
    printf("get:%d\n", res);
    printf("\nrlim_cur = %lu\n", limit.rlim_cur);
    printf("\nrlim_max = %lu\n", limit.rlim_max);
    limit.rlim_cur = sbrk(0)+10000;
    limit.rlim_max = sbrk(0)+10000;

    res = setrlimit(resource, &limit);
    printf("set:%d\n", res);
    getrlimit(resource, &limit);
    printf("\nrlim_cur = %lu\n", limit.rlim_cur);
    printf("\nrlim_max = %lu\n", limit.rlim_max);
    printf("%ld\n", sbrk(0));
    printf("........\n");
    long cond = 0;
    long int counter = 0;
    while (cond != -1)
    {
        counter++;
        cond = sbrk(100);
        // printf("%ld\n", cond);
    }
    printf("\n%ld\n", counter);
    return 0;
}
