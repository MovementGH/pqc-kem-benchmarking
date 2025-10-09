#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <api.h>
#include "benchmark.h"

#define MLEN 86
#define BENCHMARK_LENGTH 500000
#define MIN_RUNS 5

unsigned char *sk, *pk, *ct, *ss;

void setup() {
  sk = calloc (CRYPTO_SECRETKEYBYTES, sizeof (unsigned char));
  pk = calloc (CRYPTO_PUBLICKEYBYTES, sizeof (unsigned char));
  ct = calloc (CRYPTO_CIPHERTEXTBYTES, sizeof (unsigned char));
  ss = calloc (CRYPTO_BYTES, sizeof (unsigned char));
  crypto_kem_keypair (pk, sk);
}

void enc() {
  crypto_kem_enc (ct, ss, pk);
}

void dec() {
  crypto_kem_dec (ss, ct, sk);
}

int main(int argc, char** argv) {
    setup();
    printf("Algorithm: %s\n", CRYPTO_ALGNAME);
    printf("Public Key Size: %d\n", CRYPTO_PUBLICKEYBYTES);
    printf("Private Key Size: %d\n", CRYPTO_SECRETKEYBYTES);
    printf("Cipher Size: %d\n", CRYPTO_CIPHERTEXTBYTES);

    float encTime = benchmark(enc, BENCHMARK_LENGTH, MIN_RUNS);
    float decTime = benchmark(dec, BENCHMARK_LENGTH, MIN_RUNS);

    printf("Encode: %.2f/s\n", 1000000.f / encTime);
    printf("Decode: %.2f/s\n", 1000000.f / decTime);

    fprintf(stderr, "%s,%d,%d,%d,%.2f,%.2f\n", argv[0] + 2, CRYPTO_PUBLICKEYBYTES, CRYPTO_SECRETKEYBYTES, CRYPTO_CIPHERTEXTBYTES, 1000000.f / encTime, 1000000.f / decTime);
}