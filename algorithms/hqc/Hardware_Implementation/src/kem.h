/**
 * @file kem.h
 * @brief NIST KEM API used by the HQC_KEM IND-CCA2 scheme
 */

#ifndef KEM_H
#define KEM_H

#include <stdint.h>
#include <string.h>

#include "api.h"
#include "parameters.h"
#include "kem_typedef.h"

#include "hqc.h"
#include "shake_prng.h"
#include "parsing.h"
#include "shake_ds.h"
#include "shake_prng.h"
#include "vector.h"

#ifdef VERBOSE
#include <stdio.h>
#endif

int crypto_kem_all_functions_hls(ap_uint64 ss1[SHARED_SECRET_64BIT_SIZE],
                                 ap_uint64 ss2[SHARED_SECRET_64BIT_SIZE],
                                 ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                                 ap_uint8 seed_bytesize);

void crypto_kem_keypair_hls(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                            ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                            ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                            ap_uint8 seed_bytesize);

void crypto_kem_enc_hls(ap_uint8 ct[CIPHERTEXT_BYTESIZE],
                        ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                        ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                        ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                        ap_uint8 seed_bytesize);

int crypto_kem_dec_hls(ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                       ap_uint8 ct[CIPHERTEXT_BYTESIZE],
                       ap_uint64 sk[KEY_MAX_64BIT_SIZE]);

void crypto_kem_keypair_internal(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                                 ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                                 ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                                 ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
                                 // Global arrays used to store temporary data for KEM
                                 ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                                 ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                                 // Global arrays used to store temporary data for HQC
                                 vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                                 vector_byte_type store_B[VEC_N_BYTESIZE],
                                 vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                                 vector_index_type random_vector_A[PARAM_OMEGA_R],
                                 vector_index_type random_vector_B[PARAM_OMEGA_R],
                                 ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);

void crypto_kem_enc_internal(ap_uint8 ct[CRYPTO_CIPHERTEXTBYTESIZE],
                             ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                             ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                             ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                             ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
                             // Global arrays used to store temporary data for KEM
                             ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                             ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                             ap_uint64 d[SHAKE256_512_64BIT_SIZE],
                             // Global arrays used to store temporary data for HQC
                             vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                             vector_byte_type store_B[VEC_N_BYTESIZE],
                             vector_byte_type store_C[VEC_K_BYTESIZE],
                             vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                             vector_index_type random_vector_A[PARAM_OMEGA_R],
                             vector_index_type random_vector_B[PARAM_OMEGA_R],
                             ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);

int crypto_kem_dec_internal(ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                            ap_uint8 ct[CRYPTO_CIPHERTEXTBYTESIZE],
                            ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                            ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                            // Global arrays used to store temporary data for KEM
                            ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                            ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                            ap_uint64 d[SHAKE256_512_64BIT_SIZE],
                            // Global arrays used to store temporary data for HQC
                            vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                            vector_byte_type store_B[VEC_N_BYTESIZE],
                            vector_byte_type store_C[VEC_K_BYTESIZE],
                            vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                            vector_index_type random_vector_A[PARAM_OMEGA_R],
                            vector_index_type random_vector_B[PARAM_OMEGA_R],
                            ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]);

#endif // KEM_H
