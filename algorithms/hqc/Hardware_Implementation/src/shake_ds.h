#ifndef _shake_ds_
#define _shake_ds_

/**
 * @file shake_ds.h
 * @brief SHAKE-256 with incremental API and domain separation
 */

#include <stdint.h>
#include "kem_typedef.h"
#include "Keccak_Defines.h"
#include "Keccak_Stream_Absorb.h"
#include "Keccak_Squeezeblocks.h"
#include "KeccakF1600_StatePermute.h"
#include "domains.h"

void shake256_512_ds(ap_uint64 output[SHAKE256_512_64BIT_SIZE], ap_uint8 input[KECCAK_MAX_MESSAGE_BYTESIZE], ap_uint16 inlen, dom_type domain);

#endif
