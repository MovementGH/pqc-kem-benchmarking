#ifndef REED_MULLER_H
#define REED_MULLER_H

/**
 * @file reed_muller.h
 * Header file of reed_muller.cpp
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kem_typedef.h"
#include "parameters.h"
#include "vector.h"

// Copy bit 0 into all bits of an 8 bit value
#define BIT0MASK_8BITS(x) (int8_t)(-((x) & 1))

void encode_8bits(ap_uint8 word[RM_BYTESIZE], vector_byte_type message);

void hadamard(ap_int10 src[RM_BITSIZE], ap_int10 dst[RM_BITSIZE]);
void hadamard_loop(ap_int10 src[RM_BITSIZE], ap_int10 dst[RM_BITSIZE]);

void expand_and_sum(ap_int10 dst[RM_BITSIZE], vector_byte_type src[VEC_N1N2_BYTESIZE], const ap_uint15 offset);
vector_byte_type find_peaks(ap_int10 transform[RM_BITSIZE]);

void reed_muller_encode(vector_byte_type cdw[VEC_N1N2_BYTESIZE], vector_byte_type msg[VEC_N1_BYTESIZE]);
void reed_muller_decode(vector_byte_type msg[VEC_N1_BYTESIZE], vector_byte_type cdw[VEC_N1N2_BYTESIZE]);

#endif
