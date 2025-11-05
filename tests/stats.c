#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "benchmark.h"

/**
 * @brief Comparison function for qsort to sort doubles in ascending order.
 * 
 * @param a Pointer to first double value.
 * @param b Pointer to second double value.
 * @return Negative if a < b, zero if a == b, positive if a > b.
 */
int compare_double(const void *a, const void *b) {
    double diff = *(double*)a - *(double*)b;
    return (diff > 0) - (diff < 0);
}

/**
 * @brief Removes outliers from the dataset by trimming top and bottom percentiles.
 * 
 * Sorts the data array and removes OUTLIER_PERCENTAGE from both ends of the distribution.
 * The remaining data is moved to the beginning of the array.
 * 
 * @param data Array of timing measurements in microseconds.
 * @param n Number of elements in the data array.
 * @return New size of the array after outlier removal.
 */
int remove_outliers(double *data, int n) {
    qsort(data, n, sizeof(double), compare_double);

    // Remove outliers
    int remove = (int)(n * OUTLIER_PERCENTAGE / 100.0);
    if (remove == 0) return n;

    // Calc and memmove new num of data
    int new_n = n - 2 * remove;
    memmove(data, data + remove, new_n * sizeof(double));

    return new_n;
}

/**
 * @brief Calculates and prints comprehensive statistics for timing measurements.
 * 
 * Computes mean, standard deviation, median, and various percentiles from the
 * timing data and displays them in a formatted output.
 * 
 * @param operation Name of the operation being measured (e.g., "KeyGen").
 * @param data Sorted array of timing measurements in microseconds.
 * @param n Number of elements in the data array.
 */
void print_stats(const char *operation, double *data, int n) {
    
    // Sum and calculate mean
    double sum = 0;
    for (int i = 0; i < n; i++) sum += data[i];
    double mean = sum / n;
    
    // Calcular variance and stdev
    double var = 0;
    for (int i = 0; i < n; i++) {
        double diff = data[i] - mean;
        var += diff * diff;
    }
    double stdev = sqrt(var / n);
    
    // Printing operations (couldn't be bothered thx chat)
    printf("\n%s:\n", operation);
    printf("  Samples:    %d (after outlier removal)\n", n);
    printf("  Median:     %.2f μs (%.0f ops/sec)\n", data[n/2], 1000000.0 / data[n/2]);
    printf("  Mean:       %.2f μs\n", mean);
    printf("  Std Dev:    %.2f μs\n", stdev);
    printf("  10th %%ile:  %.2f μs\n", data[(int)(n * 0.10)]);
    printf("  25th %%ile:  %.2f μs\n", data[(int)(n * 0.25)]);
    printf("  75th %%ile:  %.2f μs\n", data[(int)(n * 0.75)]);
    printf("  90th %%ile:  %.2f μs\n", data[(int)(n * 0.90)]);
    printf("  95th %%ile:  %.2f μs\n", data[(int)(n * 0.95)]);
}