#ifndef VECTOR_H
#define VECTOR_H

/**
 * @file vector.h
 * @brief Header file for vector.cpp
 */


#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "kem_typedef.h"
#include "parameters.h"
#include "shake_prng.h"


#ifndef HLS_DATATYPES
#include <immintrin.h>
#endif



void vect_set_random_fixed_weight_by_coordinates(vector_index_type v[PARAM_OMEGA_R],
                                                 vector_weight_type weight,
                                                 ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE]);
void vect_set_random_fixed_weight(vector_byte_type v[VEC_N_BYTESIZE],
                                  vector_weight_type weight,
                                  vector_index_type fixed_weight_by_coord_vector[PARAM_OMEGA_R]);

void vect_set_random_from_prng(vector_byte_type v[VEC_K_BYTESIZE],
                               ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE],
                               ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE]);

void vect_set_random_mult_type(vector_word_type v[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                               ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE]);

void vect_add(vector_byte_type o[VEC_N_BYTESIZE],
              vector_byte_type v1[VEC_N_BYTESIZE],
              vector_byte_type v2[VEC_N_BYTESIZE],
              uint16_t size);
 
bool vect_compare(vector_byte_type v1[VEC_N_BYTESIZE], vector_byte_type v2[VEC_N_BYTESIZE], vector_size_type size);
bool vect_compare_64(ap_uint64 v1[SHAKE256_512_64BIT_SIZE], ap_uint64 v2[SHAKE256_512_64BIT_SIZE], vector_size_type size) ;

void vector_init(vector_byte_type array[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE], uint16_t size);
void vector_init_64(ap_uint64 array[SECRET_KEY_64BIT_SIZE], uint16_t size);

void vect_copyresize(vector_byte_type o[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], uint16_t size_o, const vector_byte_type v[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], uint16_t size_v);
void vect_copyresize_64(ap_uint64 o[KEY_MAX_64BIT_SIZE], uint8_t size_o, const ap_uint64 v[KEY_MAX_64BIT_SIZE], uint8_t size_v);
void vect_copyresize_mult(vector_word_type o[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER], const vector_byte_type v[VEC_N_BYTESIZE]);

#ifdef VERBOSE
void vect_print(const uint8_t *v, const uint32_t size);
void vect_print_sparse(const uint16_t *v, const uint16_t weight);
#endif

#endif //VECTOR_H
