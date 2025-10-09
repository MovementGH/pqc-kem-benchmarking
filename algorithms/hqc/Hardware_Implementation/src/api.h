/**
 * @file api.h
 * @brief NIST KEM API used by the HQC_KEM IND-CCA2 scheme
 */

#ifndef API_H
#define API_H

#define CRYPTO_ALGNAME                      "HQC-128"

#define CRYPTO_SECRETKEYBYTESIZE               2289
#define CRYPTO_PUBLICKEYBYTESIZE               2249
#define CRYPTO_SHARED_SECRET_BYTESIZE          64
#define CRYPTO_CIPHERTEXTBYTESIZE              4481

// As a technicality, the public key is appended to the secret key in order to respect the NIST API.
// Without this constraint, CRYPTO_SECRETKEYBYTESIZE would be defined as 32

#include "stdint.h"

void crypto_kem_keypair(uint8_t pk[CRYPTO_PUBLICKEYBYTESIZE],
                        uint8_t sk[CRYPTO_SECRETKEYBYTESIZE],
                        uint8_t *prng_seed,
                        uint8_t seed_bytesize);

void crypto_kem_enc(uint8_t ct[CRYPTO_CIPHERTEXTBYTESIZE],
                    uint8_t ss[CRYPTO_SHARED_SECRET_BYTESIZE],
                    uint8_t pk[CRYPTO_PUBLICKEYBYTESIZE],
                    uint8_t *prng_seed,
                    uint8_t seed_bytesize);

int crypto_kem_dec(uint8_t ss[CRYPTO_SHARED_SECRET_BYTESIZE],
                   uint8_t ct[CRYPTO_CIPHERTEXTBYTESIZE],
                   uint8_t sk[CRYPTO_SECRETKEYBYTESIZE]);

#endif
