#ifndef _BENCH_H_
#define _BENCH_H_

#include <stdio.h>
#include <stdlib.h>

// test configs
#define WARMUP_ITERATIONS 1000
#define MEASUREMENT_ITERATIONS 10000
#define OUTLIER_PERCENTAGE 10

// global buffers
extern unsigned char *sk, *pk, *ct, *ss;
extern double *timings;

// stats funcs
int compare_double(const void *a, const void *b);
int remove_outliers(double *data, int n);
void print_stats(const char *operation, double *data, int n);

// KEM op wrappers
void setup();
void keygen();
void enc();
void dec();

// Benchmark function (no longer used in main, kept for compatibility)
typedef void (*function_t)();
float benchmark(function_t function, int duration, int minRuns);

// new benchmark function
void benchmark_operation(function_t operation, double *data, int do_warmup);

#endif