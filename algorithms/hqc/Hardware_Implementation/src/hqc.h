#ifndef HQC_H
#define HQC_H

/**
 * @file hqc.h
 * @brief Functions of the HQC_PKE IND_CPA scheme
 */
#include <stdint.h>

#include "kem_typedef.h"
#include "parameters.h"
#include "gf2x.h"
#include "parsing.h"
#include "code.h"
#include "vector.h"
#include "shake_prng.h"

#ifdef VERBOSE
#include <stdio.h>
#endif

void hqc_pke_keygen(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                    ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                    ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
	                // Global arrays used to store temporary data
	                vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                    vector_byte_type store_B[VEC_N_BYTESIZE],
                    vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                    vector_index_type random_vector_A[PARAM_OMEGA_R],
                    vector_index_type random_vector_B[PARAM_OMEGA_R],
                    ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);

void hqc_pke_encrypt(vector_byte_type u[VEC_N_BYTESIZE],
                     vector_byte_type v[VEC_N1N2_BYTESIZE],
                     vector_byte_type m[VEC_K_BYTESIZE],
                     ap_uint64 theta[KEM_SEED_64BIT_SIZE],
                     ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                     ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
	                 // Global arrays used to store temporary data
	                 vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                     vector_byte_type store_B[VEC_N_BYTESIZE],
                     vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                     vector_index_type random_vector_A[PARAM_OMEGA_R],
                     vector_index_type random_vector_B[PARAM_OMEGA_R],
                     ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);
	
void hqc_pke_decrypt(vector_byte_type m[VEC_K_BYTESIZE],
                     vector_byte_type u[VEC_N_BYTESIZE],
                     vector_byte_type v[VEC_N1N2_BYTESIZE],
                     ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                     ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                     // Global arrays used to store temporary data
                     vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                     vector_byte_type store_B[VEC_N_BYTESIZE],
                     vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                     vector_index_type random_vector_A[PARAM_OMEGA_R],
                     ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);

#endif //HQC_H

