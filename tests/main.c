#include "benchmark.h"
#include "api.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// Test configuration
#define WARMUP_ITERATIONS 1000
#define MEASUREMENT_ITERATIONS 10000
#define OUTLIER_PERCENTAGE 10

// Global buffers - define them here
static unsigned char sk[CRYPTO_SECRETKEYBYTES];
static unsigned char pk[CRYPTO_PUBLICKEYBYTES];
static unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
static unsigned char ss[CRYPTO_BYTES];

/**
 * @brief Wrapper function for KEM key generation.
 *
 * Generates a new keypair and stores it in the global pk and sk buffers.
 */
void keygen() {
    crypto_kem_keypair(pk, sk);
}

/**
 * @brief Wrapper function for KEM encapsulation.
 *
 * Encapsulates a shared secret using the public key and stores the ciphertext
 * and shared secret in the global ct and ss buffers.
 */
void enc() {
    crypto_kem_enc(ct, ss, pk);
}

/**
 * @brief Wrapper function for KEM decapsulation.
 *
 * Decapsulates the shared secret from the ciphertext using the secret key
 * and stores the result in the global ss buffer.
 */
void dec() {
    crypto_kem_dec(ss, ct, sk);
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
void print_distribution(const char* label, uint64_t* data, size_t data_len) {

    // Sum and calculate mean
    double sum = 0;
    for (size_t i = 0; i < data_len; i++)
        sum += data[i];
    double mean = sum / data_len;

    // Calcular variance and stdev
    double var = 0;
    for (size_t i = 0; i < data_len; i++)
        var += pow(data[i] - mean, 2);
    double stdev = sqrt(var / data_len);

    // Printing operations (couldn't be bothered thx chat)
    printf("\n%s:\n", label);
    printf("  Samples:    %7d (after outlier removal)\n", data_len);
    printf("  Median:     %7d ns (%d ops/sec)\n", data[data_len / 2], NS_PER_SEC / data[data_len / 2]);
    printf("  Mean:       %7d ns\n", (uint64_t)mean);
    printf("  Std Dev:    %7d ns\n", (uint64_t)stdev);
    printf("  10th %%ile:  %7d ns\n", data[(size_t)(data_len * 0.10)]);
    printf("  25th %%ile:  %7d ns\n", data[(size_t)(data_len * 0.25)]);
    printf("  75th %%ile:  %7d ns\n", data[(size_t)(data_len * 0.75)]);
    printf("  90th %%ile:  %7d ns\n", data[(size_t)(data_len * 0.90)]);
    printf("  95th %%ile:  %7d ns\n", data[(size_t)(data_len * 0.95)]);
}

int main(int argc, char** argv) {
    // Validate the algos correctness
    unsigned char ss_check[CRYPTO_BYTES];
    crypto_kem_keypair(pk, sk);
    crypto_kem_enc(ct, ss, pk);
    crypto_kem_dec(ss_check, ct, sk);
    if (memcmp(ss, ss_check, CRYPTO_BYTES) != 0) {
        printf("ERROR: Shared secrets don't match!\n");
        return -1;
    }

    // Header and static data
    printf("=====================================\n");
    printf("PQC KEM Isolated Benchmark\n");
    printf("Following Paquin et al. (2020) & Becker et al. (2024)\n");
    printf("=====================================\n");
    printf("Algorithm:   %s\n", CRYPTO_ALGNAME);
    printf("Public Key:  %7d bytes\n", CRYPTO_PUBLICKEYBYTES);
    printf("Secret Key:  %7d bytes\n", CRYPTO_SECRETKEYBYTES);
    printf("Ciphertext:  %7d bytes\n", CRYPTO_CIPHERTEXTBYTES);
    printf("=====================================\n");
    printf("Warmup:      %7d iterations\n", WARMUP_ITERATIONS);
    printf("Measurement: %7d iterations\n", MEASUREMENT_ITERATIONS);
    printf("Outliers:    Remove top/bottom %7d%%\n", OUTLIER_PERCENTAGE);
    printf("=====================================\n");

    uint64_t timings[MEASUREMENT_ITERATIONS];
    uint64_t timings_len = MEASUREMENT_ITERATIONS;

    // Benchmark KeyGen
    printf("\nPhase 1: Key Generation:\n");
    benchmark(keygen, timings, WARMUP_ITERATIONS, MEASUREMENT_ITERATIONS);
    timings_len = remove_outliers(timings, MEASUREMENT_ITERATIONS, OUTLIER_PERCENTAGE);
    print_distribution("KeyGen", timings, timings_len);
    uint64_t keygen_median = timings[timings_len / 2];

    // Benchmark Encaps
    printf("\nPhase 2: Encapsulation:\n");
    benchmark(enc, timings, WARMUP_ITERATIONS, MEASUREMENT_ITERATIONS);
    timings_len = remove_outliers(timings, MEASUREMENT_ITERATIONS, OUTLIER_PERCENTAGE);
    print_distribution("Encapsulation", timings, timings_len);
    uint64_t encaps_median = timings[timings_len / 2];

    // Benchmark Decaps
    printf("\nPhase 3: Decapsulation:\n");
    benchmark(dec, timings, WARMUP_ITERATIONS, MEASUREMENT_ITERATIONS);
    timings_len = remove_outliers(timings, MEASUREMENT_ITERATIONS, OUTLIER_PERCENTAGE);
    print_distribution("Decapsulation", timings, timings_len);
    uint64_t decaps_median = timings[timings_len / 2];

    // Summary
    printf("\n=====================================\n");
    printf("SUMMARY (Median Times)\n");
    printf("=====================================\n");
    printf("KeyGen:           %7d ns\n", keygen_median);
    printf("Encapsulation:    %7d ns\n", encaps_median);
    printf("Decapsulation:    %7d ns\n", decaps_median);
    printf("Total Handshake:  %7d ns (Encaps + Decaps)\n", encaps_median + decaps_median);
    printf("=====================================\n");

    // CSV output to stderr for collection
    fprintf(stderr, "%s,%7d,%7d,%7d,%7d,%7d,%7d\n",
        CRYPTO_ALGNAME,
        CRYPTO_PUBLICKEYBYTES,
        CRYPTO_SECRETKEYBYTES,
        CRYPTO_CIPHERTEXTBYTES,
        keygen_median,
        decaps_median,
        encaps_median + decaps_median
    );

    return 0;
}