#ifndef CODE_H
#define CODE_H

/**
 * @file code.h
 * Header file of code.cpp
 */

#include <stddef.h>
#include <stdint.h>

#include "kem_typedef.h"
#include "parameters.h"


#include "reed_muller.h"
#include "reed_solomon.h"
#include "vector.h"
#include <stdint.h>
#include <string.h>
#ifdef VERBOSE
#include <stdio.h>
#endif




void code_encode(ap_uint8 em[VEC_N1N2_BYTESIZE], ap_uint8 m[VEC_K_BYTESIZE]);
void code_decode(ap_uint8 m[VEC_K_BYTESIZE], ap_uint8 em[VEC_N1N2_BYTESIZE]);

#endif
