# include "mysearch.h"

inline unsigned long first_eq_idx_v1(const unsigned int *array, unsigned long n, unsigned int item) {
    unsigned long idx = 0;
    for (; idx < n && array[idx] != item; ++idx);
    return idx;
}

inline unsigned long first_eq_idx_v2(const unsigned int *array, unsigned long n, unsigned int item) {
    unsigned long idx = 0;
    for (; idx < n && *array != item; ++idx, ++array);
    return idx;
}

inline unsigned long first_eq_idx_v3(unsigned int *array, unsigned long n, unsigned int item) {
    unsigned int tmp = array[n-1];
    array[n-1] = item;
    unsigned long idx = 0;
    for (; array[idx] != item; ++idx);
    array[n-1] = tmp;
    if (idx == n-1 && item != tmp)
        ++idx;
    return idx;
}

inline unsigned long first_eq_idx(unsigned int *array, unsigned long n, unsigned int item) {
    unsigned int tmp = array[n-1];
    unsigned int *head = array;
    array[n-1] = item;
    unsigned long idx = 0;
    for (; *head != item; ++idx, ++head);
    array[n-1] = tmp;
    if (idx == n-1 && item != tmp)
        ++idx;
    return idx;
}

// test begin
#ifdef __TEST_MYSEARCH

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>

long long timeInMilliseconds() {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

#define start_benchmark() start = timeInMilliseconds()
#define end_benchmark(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %lu items in %lld ms\n", count, elapsed); \
} while(0);
#define end_benchmark_failure(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %lu items in %lld ms\n", count, elapsed); \
    return -1; \
} while(0);

int main(int argc, char **argv) {
    long long start, elapsed;
    unsigned long count, idx;
    unsigned int m = 10001; // 2 << 30;
    unsigned int item = 233;

    count = argc == 2 ? strtol(argv[1], NULL, 10) : 100000000;
    unsigned int *array = (unsigned int *)malloc(sizeof(unsigned int) * count);
    array[0] = count >> 5;
    for (unsigned long i = 1; i < count; i++)
        array[i] = (7 * array[i-1] + 29) % m;  // (81 * array[i-1] + 7) % m;

    start_benchmark();
    idx = first_eq_idx_v1(array, count, item);
    if (idx != count) {
        end_benchmark("first_eq_idx_v1 (found)")
    }
    else {
        end_benchmark("first_eq_idx_v1 (not found)")
    }

    start_benchmark();
    idx = first_eq_idx_v2(array, count, item);
    if (idx != count) {
        end_benchmark("first_eq_idx_v2 (found)")
    }
    else {
        end_benchmark("first_eq_idx_v2 (not found)")
    }

    start_benchmark();
    idx = first_eq_idx_v3(array, count, item);
    if (idx != count) {
        end_benchmark("first_eq_idx_v3 (found)")
    }
    else {
        end_benchmark("first_eq_idx_v3 (not found)")
    }

    start_benchmark();
    idx = first_eq_idx(array, count, item);
    if (idx != count) {
        end_benchmark("first_eq_idx (found)")
    }
    else {
        end_benchmark("first_eq_idx (not found)")
    }
}

#endif