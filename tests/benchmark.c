#include <sys/time.h>
#include <unistd.h>
#include "benchmark.h"

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