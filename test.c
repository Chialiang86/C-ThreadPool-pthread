#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define ITERATION 1000000

void* task(void * arg)
{
    unsigned max = *(unsigned *)arg;
    unsigned long sum = 0;
    for (int i = 0; i < max; i++)
        sum += i;
    return sum;
}

int main(int argc, char * argv[]) 
{
    
    return 0;
}