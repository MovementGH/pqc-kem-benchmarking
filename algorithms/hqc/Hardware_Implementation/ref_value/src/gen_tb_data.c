/**
 * @file gen_tb_data.c
 * @brief Generates reference data for the hardware implementation testbenchs.
 */

#include <stdio.h>
#include "code.h"
#include "vector.h"
#include "hqc.h"
#include "api.h"
#include "parsing.h"
#include "shake_prng.h"
#include "shake_ds.h"

// All functions must have prototypes given HQC compilation flags
void reset_rngs(uint8_t);

/**
 * @brief Resets the PRNG.
 *
 * Resets the PRNG back to its initial state to have reproducible results.
 * Always uses the byte 0x43 as entropy for its initialisation.
 */
void reset_rngs(uint8_t entropy) {
    shake_prng_init(&entropy, NULL, 1, 0);
}
/**
 * @brief Generates reference data for the hardware implementation testbenchs.
 */
int main() {
    uint8_t entropy = 0x43;
    //RNG
    FILE* ref_data_prng = fopen("data_prng.dat", "w");
    FILE* ref_data_seedexpander = fopen("data_seedexpander.dat", "w");
    uint8_t x[16];

    //prng
    reset_rngs(entropy);
    shake_prng(x, 16);
    for (int i=0; i<16; ++i) {
        fprintf(ref_data_prng, "%02X", x[i]);
    }
    fclose(ref_data_prng);
    reset_rngs(entropy);

    //seedexpander
    seedexpander_state seedexpander_st_object;
    seedexpander_state *seedexpander_st = &seedexpander_st_object;
    uint8_t seedexpander_seed[8] = {0};
    seedexpander_seed[0] = 0x44;
    seedexpander_init(seedexpander_st, seedexpander_seed, 8);
    seedexpander(seedexpander_st, x, 16);
    for (int i=0; i<16; ++i) {
        fprintf(ref_data_seedexpander, "%02X", x[i]);
    }
    fclose(ref_data_seedexpander);
    reset_rngs(entropy);

    //KEM
    FILE* data_crypto_kem_keypair = fopen("data_crypto_kem_keypair.dat", "w");
    FILE* data_crypto_kem_enc = fopen("data_crypto_kem_enc.dat", "w");
    unsigned char pk[PUBLIC_KEY_BYTES] = {0};
    unsigned char sk[SECRET_KEY_BYTES] = {0};
    unsigned char kem_ct[CIPHERTEXT_BYTES];
    unsigned char kem_key1[SHARED_SECRET_BYTES];

    // crypto_kem_keypair
    crypto_kem_keypair(pk, sk);
    for (int i=0; i<PUBLIC_KEY_BYTES; ++i) {
        fprintf(data_crypto_kem_keypair, "%02X", pk[i]);
    }
    for (int i=0; i<SECRET_KEY_BYTES; ++i) {
        fprintf(data_crypto_kem_keypair, "%02X", sk[i]);
    }
    fclose(data_crypto_kem_keypair);
    reset_rngs(entropy);

 
    //crypto_kem_enc
    crypto_kem_keypair(pk, sk);
    reset_rngs(entropy+1);
    crypto_kem_enc(kem_ct, kem_key1, pk);
    for (int i=0; i<CIPHERTEXT_BYTES; ++i) {
        fprintf(data_crypto_kem_enc, "%02X", kem_ct[i]);
    }
    for (int i=0; i<SHARED_SECRET_BYTES; ++i) {
        fprintf(data_crypto_kem_enc, "%02X", kem_key1[i]);
    }
    fclose(data_crypto_kem_enc);

    return 0;
}
