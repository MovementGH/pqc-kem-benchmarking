#ifndef __shake_prng__
#define __shake_prng__

/**
 * @file shake_prng.h
 * @brief SHAKE-256 based PRNG and seedexpander
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "Keccak_Defines.h"
#include "Keccak_Stream_Absorb.h"
#include "Keccak_Squeezeblocks.h"
#include "KeccakF1600_StatePermute.h"
#include "kem_typedef.h"
#include "parameters.h"
#include "domains.h"
#include "vector.h"

void shake_prng_init(ap_uint8 entropy_input[SHARED_SECRET_BYTESIZE], ap_uint8 personalization_string[SHARED_SECRET_BYTESIZE], ap_uint32 enlen, ap_uint32 perlen, ap_uint64 shake_prng_state_array[PRNG_STATE_64BIT_SIZE]);
void shake_prng(ap_uint64 output[KEM_SEED_64BIT_SIZE], uint8_t outlen, ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE]);
void seedexpander_init(ap_uint64 seed[KEM_SEED_64BIT_SIZE], ap_uint16 seedlen, ap_uint64 state[PRNG_STATE_64BIT_SIZE]);
void seedexpander(vector_byte_type output[VEC_N_BYTESIZE], ap_uint16 outlen, ap_uint64 state[PRNG_STATE_64BIT_SIZE]);
void seedexpander_mult_type(vector_word_type output[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER], ap_uint16 outlen, ap_uint64 state[PRNG_STATE_64BIT_SIZE]);


#endif
