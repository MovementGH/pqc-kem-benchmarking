#ifndef _HLS_FFT_ADD_H_
#define _HLS_FFT_ADD_H_
#include <stdint.h>
#include <stddef.h>

#include <assert.h>
#include <stdio.h>

#include "api.h"
#include "parameters.h"
#include "kem_typedef.h"

#include "gf.h"

#ifdef HLS_DATATYPES
#define FFT_SHIFT 8
#else
#define FFT_SHIFT 15
#endif

void leaves_butterfly(ap_uint8 small_w[16], ap_uint8 A_0, ap_uint8 A_1);
void add_fft(ap_uint8 w[256], ap_uint8 f[32]);

void add_fft_retrieve_error_poly(ap_uint8 error[256], const ap_uint8 w[256]);
void compute_roots(ap_uint8 error[256], ap_uint8 sigma[32]);

#endif
