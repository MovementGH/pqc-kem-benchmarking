#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <api.h>
#include "benchmark.h"

/**
 * @brief Initializes the global key and ciphertext buffers and generates initial keypair.
 * 
 * Allocates memory for secret key, public key, ciphertext, and shared secret buffers
 * based on the algorithm's requirements. Also generates an initial keypair.
 */
void setup() {
    sk = calloc(CRYPTO_SECRETKEYBYTES, sizeof(unsigned char));
    pk = calloc(CRYPTO_PUBLICKEYBYTES, sizeof(unsigned char));
    ct = calloc(CRYPTO_CIPHERTEXTBYTES, sizeof(unsigned char));
    ss = calloc(CRYPTO_BYTES, sizeof(unsigned char));
    timings = malloc(MEASUREMENT_ITERATIONS * sizeof(double));

    // Generate initial keypair for enc/dec benchmarks
    crypto_kem_keypair(pk, sk);
}

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

int main(int argc, char** argv) {
    // Initialize cryptographic keys
    setup();

    // validate the algos correctness
    unsigned char ss_check[CRYPTO_BYTES];
    crypto_kem_enc(ct, ss, pk);
    crypto_kem_dec(ss_check, ct, sk);
    if (memcmp(ss, ss_check, CRYPTO_BYTES) != 0) {
        printf("ERROR: Shared secrets don't match!\n");
        return -1;
    }

    printf("=====================================\n");
    printf("PQC KEM Isolated Benchmark\n");
    printf("Following Paquin et al. (2020) & Becker et al. (2024)\n");
    printf("=====================================\n");
    printf("Algorithm:   %s\n", CRYPTO_ALGNAME);
    printf("Public Key:  %d bytes\n", CRYPTO_PUBLICKEYBYTES);
    printf("Secret Key:  %d bytes\n", CRYPTO_SECRETKEYBYTES);
    printf("Ciphertext:  %d bytes\n", CRYPTO_CIPHERTEXTBYTES);
    printf("=====================================\n");
    printf("Warmup:      %d iterations\n", WARMUP_ITERATIONS);
    printf("Measurement: %d iterations\n", MEASUREMENT_ITERATIONS);
    printf("Outliers:    Remove top/bottom %d%%\n", OUTLIER_PERCENTAGE);
    printf("Timer:       clock_gettime(CLOCK_MONOTONIC)\n");
    printf("=====================================\n");

    if (!timings) {
        printf("ERROR: Memory allocation failed\n");
        return -1;
    }

    // Benchmark KeyGen
    printf("\nPhase 1: Key Generation:\n");
    benchmark_operation(keygen, timings, 1);
    int n = remove_outliers(timings, MEASUREMENT_ITERATIONS);
    print_stats("KeyGen", timings, n);
    double keygen_median = timings[n/2];

    // Benchmark Encaps
    printf("\nPhase 2: Encapsulation:\n");
    benchmark_operation(enc, timings, 0);
    n = remove_outliers(timings, MEASUREMENT_ITERATIONS);
    print_stats("Encapsulation", timings, n);
    double encaps_median = timings[n/2];

    // Benchmark Decaps
    printf("\nPhase 3: Decapsulation:\n");
    benchmark_operation(dec, timings, 0);
    n = remove_outliers(timings, MEASUREMENT_ITERATIONS);
    print_stats("Decapsulation", timings, n);
    double decaps_median = timings[n/2];

    // Summary
    printf("\n=====================================\n");
    printf("SUMMARY (Median Times)\n");
    printf("=====================================\n");
    printf("KeyGen:           %.2f μs\n", keygen_median);
    printf("Encapsulation:    %.2f μs\n", encaps_median);
    printf("Decapsulation:    %.2f μs\n", decaps_median);
    printf("Total Handshake:  %.2f μs (Encaps + Decaps)\n", encaps_median + decaps_median);
    printf("=====================================\n");

    // CSV output to stderr for collection
    fprintf(stderr, "%s,%dbytes,%dbytes,%dbytes,%.2fus,%.2fus,%.2fus,%.2fus\n", 
            CRYPTO_ALGNAME, 
            CRYPTO_PUBLICKEYBYTES, 
            CRYPTO_SECRETKEYBYTES, 
            CRYPTO_CIPHERTEXTBYTES,
            keygen_median, 
            encaps_median, 
            decaps_median, 
            encaps_median + decaps_median);

    // Cleanup
    free(timings);
    free(sk);
    free(pk);
    free(ct);
    free(ss);

    return 0;
}