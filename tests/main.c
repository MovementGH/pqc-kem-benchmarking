#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <api.h>
#include "benchmark.h"

#define MLEN 86
#define BENCHMARK_LENGTH 500000
#define MIN_RUNS 5

unsigned char *sk, *pk, *ct, *ss;

/**
 * @brief Initializes the cryptographic keys and generates a new key pair.
 */
void setup() {
    sk = calloc(CRYPTO_SECRETKEYBYTES, sizeof(unsigned char));
    pk = calloc(CRYPTO_PUBLICKEYBYTES, sizeof(unsigned char));
    ct = calloc(CRYPTO_CIPHERTEXTBYTES, sizeof(unsigned char));
    ss = calloc(CRYPTO_BYTES, sizeof(unsigned char));

    // Generate a new key pair
    crypto_kem_keypair(pk, sk);
}

/**
 * @brief Encapsulates a key using the KEM.
 */
void enc() {
    crypto_kem_enc(ct, ss, pk);
}

/**
 * @brief Decapsulates a key using the KEM
 */
void dec() {
    crypto_kem_dec(ss, ct, sk);
}

int main(int argc, char** argv) {
    // Initialize cryptographic keys
    setup();

    // Print out the details of the KEM
    printf("Algorithm: %s\n", CRYPTO_ALGNAME);
    printf("Public Key Size: %d\n", CRYPTO_PUBLICKEYBYTES);
    printf("Private Key Size: %d\n", CRYPTO_SECRETKEYBYTES);
    printf("Cipher Size: %d\n", CRYPTO_CIPHERTEXTBYTES);

    // Benchmark the encryption function
    float encTime = benchmark(enc, BENCHMARK_LENGTH, MIN_RUNS);

    // Benchmark the decryption function
    float decTime = benchmark(dec, BENCHMARK_LENGTH, MIN_RUNS);

    // Output the results of the benchmarks in human readable format
    printf("Encode: %.2f/s\n", 1000000.0f / encTime);
    printf("Decode: %.2f/s\n", 1000000.0f / decTime);

    // stderr is captured into a csv, so print all the metrics in that format as well.
    fprintf(stderr, "%s,%d,%d,%d,%.2f,%.2f\n", argv[0] + 2, CRYPTO_PUBLICKEYBYTES, CRYPTO_SECRETKEYBYTES, CRYPTO_CIPHERTEXTBYTES, 1000000.0f / encTime, 1000000.0f / decTime);

}