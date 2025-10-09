#ifndef _KeccakF1600_StatePermute_h_
#define _KeccakF1600_StatePermute_h_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "Keccak_Defines.h"
#include "parameters.h"
#include "kem_typedef.h"




void KeccakF1600_StatePermute(ap_uint64 state[KECCAK_STATE_64BIT_SIZE]);
void PermuteCompact(ap_uint64 A[KECCAK_STATE_64BIT_SIZE], ap_uint64 E[KECCAK_STATE_64BIT_SIZE], unsigned int round);

#endif
