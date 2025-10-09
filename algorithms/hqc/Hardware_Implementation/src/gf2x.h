#ifndef GF2X_H
#define GF2X_H

/**
 * @file gf2x.h
 * @brief Header file for gf2x.cpp
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "parameters.h"
#include "kem_typedef.h"



void vect_mul_add(vector_byte_type o[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], vector_index_type a1[PARAM_OMEGA_R], vector_word_type a2[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1], ap_uint7 weight, vector_byte_type a3[VEC_N_BYTESIZE]);
void reduce_naive_add(vector_byte_type o[VEC_N_BYTESIZE], multiplier_word_type a1[VEC_N_MULTIPLIERWORDSIZE << 1], vector_byte_type a2[VEC_N_BYTESIZE]);
void naive_convolution_mult(multiplier_word_type o[VEC_N_MULTIPLIERWORDSIZE << 1], vector_index_type a1[PARAM_OMEGA_R], multiplier_word_type a2[VEC_N_MULTIPLIERWORDSIZE + 1], ap_uint7 weight);
void vector_init_res_mult(multiplier_word_type array[VEC_N_MULTIPLIERWORDSIZE << 1]);



#endif