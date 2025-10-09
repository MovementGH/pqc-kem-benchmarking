#ifndef _Keccak_Stream_Absorb_h_
#define _Keccak_Stream_Absorb_h_

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "kem_typedef.h"
#include "parameters.h"

#include "Keccak_Defines.h"

#include "KeccakF1600_StatePermute.h"



void Keccak_Stream_Absorb_Finalize(ap_uint64 state[PRNG_STATE_64BIT_SIZE], ap_uint8 p);
void Keccak_Stream_Absorb_With_DS_From_Byte_0(ap_uint64 state[26], ap_uint8 input[KECCAK_MAX_MESSAGE_BYTESIZE], ap_uint64 input64[KECCAK_MAX_MESSAGE_BYTESIZE / 8], ap_uint1 usewidedata, ap_uint16 inlen, dom_type domain);



#endif
