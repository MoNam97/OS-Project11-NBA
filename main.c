#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
    int *p = (int *)malloc(10*sizeof(int));
    long long start = (long long)p;
    long long end = (long long)sbrk(0);
    printf("%lld - %lld\n", start,(long long int)p + 10*sizeof(int));
    printf("%lld\n%lld\n", (long long)sbrk(0), (long long)sbrk(0) - start);
    long long int *  temp = (long long int *)malloc(100000000*sizeof(long long int));
    printf("%lld\n", (long long)sbrk(0));
    return 0;
}
