#ifndef PARSING_H
#define PARSING_H

/**
 * @file parsing.h
 * @brief Header file for parsing.cpp
 */
#include <stdint.h>
#include <string.h>

#include "kem_typedef.h"
#include "parameters.h"

#include "shake_prng.h"
#include "vector.h"

void hqc_secret_key_to_string(ap_uint64 sk[SECRET_KEY_BYTESIZE], ap_uint64 pk[PUBLIC_KEY_BYTESIZE]);
void hqc_secret_key_from_string(vector_index_type y[PARAM_OMEGA], ap_uint64 sk[KEM_SEED_64BIT_SIZE], ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE]);

	
void hqc_public_key_from_string(vector_word_type s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER], ap_uint64 pk[PUBLIC_KEY_64BIT_SIZE]);
void hqc_public_key_to_string(ap_uint64 pk[SECRET_KEY_64BIT_SIZE], vector_byte_type s[VEC_N_BYTESIZE]);


void hqc_ciphertext_to_string_64(vector_byte_type u[VEC_N_BYTESIZE], vector_byte_type v[VEC_N1N2_BYTESIZE], ap_uint64 d[SHAKE256_512_64BIT_SIZE], vector_byte_type ct[CIPHERTEXT_BYTESIZE]);
void hqc_ciphertext_from_string_64(vector_byte_type u[VEC_N_BYTESIZE], vector_byte_type v[VEC_N1N2_BYTESIZE], ap_uint64 d[SHAKE256_512_64BIT_SIZE], vector_byte_type ct[CIPHERTEXT_BYTESIZE]);

#endif //PARSING_H

