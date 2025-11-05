#include <unistd.h>
#include <time.h> // Comment this out if clock_gettime isn't working
#include <sys/time.h>
#include "benchmark.h"

// Global buffers - define them here
unsigned char *sk = NULL;
unsigned char *pk = NULL;
unsigned char *ct = NULL;
unsigned char *ss = NULL;
double *timings = NULL;

/**
 * This function measures the elapsed time for running a given function
 * multiple times.
 *
 * @param func Pointer to the function that needs to be timed.
 * @param runs Number of times the function should be called for measuring the time.
 * @return Elapsed execution time in microseconds.
 */
int getTime(function_t func, int runs) {
    struct timeval start, end;

    // Record the current time before running the function
    gettimeofday(&start, NULL);

    for(int i = 0; i < runs; i++)
        func();

    gettimeofday(&end, NULL);

    // Calculate the elapsed time in microseconds
    return (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec);
}

/**
 * @brief Benchmarks a function over multiple runs and returns an average execution time.
 *
 * This function repeatedly calls the provided function until it achieves
 * a benchmarking duration within a specified tolerance. It adjusts the number of runs
 * dynamically based on the initial execution time to get as close as possible
 * to the desired duration while making sure to meet a minimum run count.
 *
 *
 * @param function Pointer to the function to be benchmarked.
 * @param duration Desired benchmarking duration in seconds.
 * @param minRuns Minimum number of runs allowed, even if it takes more than 'duration' seconds.
 * @return Average execution time per call in microseconds.
 */
float benchmark(function_t function, int duration, int minRuns) {
    int runs = minRuns, runTime = 0;

    for(int i = 0; i < 10; i++) {
        runTime = getTime(function, runs);

        // Check if the measured time is within 5% of the desired duration
        if(runTime > duration * .95f && runTime < duration * 1.05f)
            break;

        // If the function takes too long to run, adjust the number of runs for subsequent tests
        runs *= (float)duration / (float)runTime;

        // Ensure that we don't fall below the minimum number of runs
        if(runs < minRuns) {
            runs = minRuns;
            break;
        }
    }

    // Measure the time again with the final calculated number of runs
    runTime = getTime(function, runs);

    return (float)runTime / (float)runs;
}

/**
 * @brief Benchmarks a KEM operation with warmup and measurement, storing results in data array.
 *        I have optional comments if clock_gettime isn't working
 * 
 * This function performs:
 * 1. Optional warmup iterations
 * 2. High-precision timing measurements using CLOCK_MONOTONIC for MEASUREMENT_ITERATIONS
 * 3. Stores raw timing data in the provided data array
 * 
 * @param operation Function pointer to the operation to benchmark (keygen, enc, dec).
 * @param data Array to store timing measurements (must be at least MEASUREMENT_ITERATIONS size).
 * @param do_warmup Whether to perform warmup iterations (1 = yes, 0 = no).
 */
void benchmark_operation(function_t operation, double *data, int do_warmup) {
    struct timespec start, end;
    //struct timeval start, end;

    // Warmup phase
    if (do_warmup) {
        for (int i = 0; i < WARMUP_ITERATIONS; i++)
            operation();
    }
    
    // Measurement phase
    for (int i = 0; i < MEASUREMENT_ITERATIONS; i++) {

        clock_gettime(CLOCK_MONOTONIC, &start);
        //gettimeofday(&start, NULL);

        operation();

        clock_gettime(CLOCK_MONOTONIC, &end);
        //gettimeofday(&end, NULL);
        
        data[i] = (end.tv_sec - start.tv_sec) * 1000000.0 +
                  (end.tv_nsec - start.tv_nsec) / 1000.0;
        //data[i] = (end.tv_sec - start.tv_sec) * 1000 * 1000 + (end.tv_usec - start.tv_usec);
    }
}