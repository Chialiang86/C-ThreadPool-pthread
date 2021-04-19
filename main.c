#include "threadpool.h"

#define PRECISION 100 /* upper bound in BPP sum */
#define SECONDS 1
#define NINO 1000000000

/* Use Bailey–Borwein–Plouffe formula to approximate PI */
static void *bpp(void *arg)
{
    unsigned a = 0;
    for (int i = 0; i < 100000; i++)
        a++;
    int k = *(int *) arg;
    double sum = (4.0 / (8 * k + 1)) - (2.0 / (8 * k + 4)) -
                 (1.0 / (8 * k + 5)) - (1.0 / (8 * k + 6));
    double *product = malloc(sizeof(double));
    if (product)
        *product = 1 / pow(16, k) * sum;
    return (void *) product;
}

static struct timespec time_cnt(struct timespec *start, struct timespec *end)
{
    struct timespec ret;
    if (end->tv_nsec - start->tv_nsec < 0) {
        ret.tv_sec = end->tv_sec - start->tv_sec - 1;
        ret.tv_nsec = NINO + end->tv_nsec - start->tv_nsec;
    } else {
        ret.tv_sec = end->tv_sec - start->tv_sec;
        ret.tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return ret;
}

int main(int argc, char * argv[])
{
    int bpp_args[PRECISION + 1];
	int thread_num = atoi(argv[1]);
    double bpp_sum = 0;
    tpool_t pool = tpool_create(thread_num);
    tpool_future_t futures[PRECISION + 1];


    struct timespec start, end, diff;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i <= PRECISION; i++) {
        bpp_args[i] = i;
        futures[i] = tpool_apply(pool, bpp, (void *) &bpp_args[i]);
    }

    for (int i = 0; i <= PRECISION; i++) {
        double *result;
        while(!(result = tpool_future_get(pool, futures[i], SECONDS /* blocking wait */))) {
            futures[i] = tpool_apply(pool, bpp, (void *) &bpp_args[i]);
            struct timespec expire_time;
            clock_gettime(CLOCK_MONOTONIC, &expire_time);
            expire_time.tv_sec += 1;
            for (struct timespec tmp; tmp.tv_sec < expire_time.tv_sec; clock_gettime(CLOCK_MONOTONIC, &tmp));
        }
        bpp_sum += *result;
        tpool_future_destroy(futures[i]);
        free(result);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    diff = time_cnt(&start, &end);
    double time = (double)diff.tv_sec + (double)diff.tv_nsec / NINO ;

    tpool_join(pool);
    printf("PI calculated with %d terms: %.15f\n", PRECISION + 1, bpp_sum);
    printf("Total time = %f \n", time);
    return 0;
}
