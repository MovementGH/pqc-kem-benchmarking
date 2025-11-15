#ifndef _BENCH_H_
#define _BENCH_H_
#include <stdint.h>
#include <stddef.h>

#define NS_PER_SEC 1000000000l
#define US_PER_SEC 1000000l
#define MS_PER_SEC 1000l

// Benchmark funcs
typedef void (*function_t)();
void benchmark(function_t operation, uint64_t* data, size_t warmup_iters, size_t measure_iters);

// Data processing funcs
size_t remove_outliers(uint64_t* data, size_t data_len, uint8_t outlier_percentage);
void print_stats(const char* label, uint64_t* data, size_t data_len);

#endif