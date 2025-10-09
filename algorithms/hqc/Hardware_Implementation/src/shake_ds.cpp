/**
 * @file shake_ds.cpp
 * @brief Implementation of shake_ds.h
 */

#include "shake_ds.h"


/**
 * @brief SHAKE-256 random oracle with domain separation
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[in] output Pointer to ap_uint64 output
 * @param[in] input Pointer to ap_uint8 input
 * @param[in] inlen length of input in bytes
 * @param[in] domain byte for domain separation
 */
void shake256_512_ds(ap_uint64 output[SHAKE256_512_64BIT_SIZE],
                     ap_uint8 input[KECCAK_MAX_MESSAGE_BYTESIZE],
                     ap_uint16 inlen,
                     dom_type domain)
{
    static ap_uint64 state[PRNG_STATE_64BIT_SIZE];
    ap_uint16 j;

    /* Init state */
    memset : for (j = 0; j < PRNG_STATE_64BIT_SIZE; ++j) state[j] = 0;

    /* Absorb input and domain separation byte*/
    Keccak_Stream_Absorb_With_DS_From_Byte_0(state, input, NULL, 0, inlen, domain);

    /* Finalize */
    Keccak_Stream_Absorb_Finalize(state, 0x1F);

    /* Squeeze output */
    Keccak_Squeezeblocks_512Bits(output, state);

}
