#ifndef ECDH_TLS_KEM_H
#define ECDH_TLS_KEM_H

#include <stdint.h>

#define CRYPTO_ALGNAME             "ECDH-256"
#define CRYPTO_PUBLICKEYBYTES      65
#define CRYPTO_SECRETKEYBYTES      32
#define CRYPTO_BYTES               32
#define CRYPTO_CIPHERTEXTBYTES     65

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif