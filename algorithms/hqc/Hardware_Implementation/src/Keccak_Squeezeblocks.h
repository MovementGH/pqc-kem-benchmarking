#ifndef _Keccak_Squeezeblocks_h_
#define _Keccak_Squeezeblocks_h_

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include "kem_typedef.h"
#include "parameters.h"
#include "Keccak_Defines.h"

#include "KeccakF1600_StatePermute.h"


void Keccak_Squeezeblocks_512Bits(ap_uint64 h[SHAKE256_512_64BIT_SIZE], ap_uint64 state[PRNG_STATE_64BIT_SIZE]);
void Keccak_Squeezeblocks_64_Inc(ap_uint64 h[PRNG_MAX_OUTPUT_BYTESIZE], uint8_t outputsize, ap_uint64 state[PRNG_STATE_64BIT_SIZE], ap_uint8 rate);

#endif
