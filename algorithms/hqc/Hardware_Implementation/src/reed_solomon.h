/**
 * @file reed_solomon.h
 * Header file of reed_solomon.cpp
 */

#ifndef REED_SOLOMON_H
#define REED_SOLOMON_H

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "parameters.h"
#include "kem_typedef.h"
#include "hls_fft_add.h"
#include "gf.h"
#include "vector.h"

#ifdef VERBOSE
#include <stdbool.h>
#include <stdio.h>
#endif

#ifdef HLS_DATATYPES
#define RS_SHIFT_ROOT 8
#else
#define RS_SHIFT_ROOT 15
#endif

void reed_solomon_encode(ap_uint8 cdw[VEC_N1_BYTESIZE], ap_uint8 msg[VEC_K_BYTESIZE]);
void reed_solomon_decode(ap_uint8 msg[VEC_K_BYTESIZE], ap_uint8 cdw[VEC_N1_BYTESIZE]);


void compute_syndromes(ap_uint8 syndromes[2 * PARAM_DELTA], ap_uint8 cdw[VEC_N1_BYTESIZE]);
ap_uint8 compute_elp(ap_uint8 sigma[1 << PARAM_FFT], const ap_uint8 syndromes[2 * PARAM_DELTA]);
void compute_z_poly(ap_uint8 z[PARAM_DELTA + 1], const ap_uint8 sigma[1 << PARAM_FFT], const ap_uint8 degree, const ap_uint8 syndromes[2 * PARAM_DELTA]);
void compute_error_values(ap_uint8 error_values[PARAM_DELTA], ap_uint8 z[PARAM_DELTA + 1], ap_uint8 error[VEC_N1_BYTESIZE]);
void correct_errors(ap_uint8 cdw[VEC_N1_BYTESIZE], const ap_uint8 error_values[VEC_N1_BYTESIZE]);

#endif
