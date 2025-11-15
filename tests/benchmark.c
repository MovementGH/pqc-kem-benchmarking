#include "benchmark.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Benchmarks a KEM fn with warmup and measurement, storing results in data array.
 *
 * This function performs:
 * 1. Optional warmup iterations
 * 2. High-precision timing measurements using CLOCK_MONOTONIC for MEASUREMENT_ITERATIONS
 * 3. Stores raw timing data in the provided data array
 *
 * @param fn Function pointer to the fn to benchmark (keygen, enc, dec).
 * @param data Array to store timing measurements (must be at least MEASUREMENT_ITERATIONS size).
 * @param do_warmup Whether to perform warmup iterations (1 = yes, 0 = no).
 */
void benchmark(function_t fn, uint64_t* data, size_t warmup_iters, size_t measure_iters) {
    struct timespec start, end;

    for (size_t i = 0; i < warmup_iters; i++)
        fn();

    // Measurement phase
    for (size_t i = 0; i < measure_iters; i++) {

        clock_gettime(CLOCK_MONOTONIC, &start);
        fn();
        clock_gettime(CLOCK_MONOTONIC, &end);

        data[i] = (end.tv_sec - start.tv_sec) * NS_PER_SEC + (end.tv_nsec - start.tv_nsec);
    }
}

/**
 * @brief Comparison function for qsort to sort uint64_t in ascending order.
 *
 * @param a Pointer to first uint64_t value.
 * @param b Pointer to second uint64_t value.
 * @return Negative if a < b, zero if a == b, positive if a > b.
 */
int compare_uint64(const void* raw_a, const void* raw_b) {
    const uint64_t* a = raw_a;
    const uint64_t* b = raw_b;
    return (*a > *b) - (*a < *b);
}

/**
 * @brief Removes outliers from the dataset by trimming top and bottom percentiles.
 *
 * Sorts the data array and removes outlier_percentage from both ends of the distribution.
 * The remaining data is moved to the beginning of the array.
 *
 * @param data Array of data.
 * @param n Number of elements in the data array.
 * @return New size of the array after outlier removal.
 */
size_t remove_outliers(uint64_t* data, size_t data_len, uint8_t outlier_percentage) {
    qsort(data, data_len, sizeof(*data), compare_uint64);

    // Remove outliers
    size_t remove = data_len * outlier_percentage / 100;
    if (remove == 0)
        return data_len;

    // Calculate how many should remain after removing the proper
    // percentage and shift the data to the beginning of the array
    size_t new_len = data_len - (2 * remove);
    memmove(data, data + remove, new_len * sizeof(*data));

    return new_len;
}