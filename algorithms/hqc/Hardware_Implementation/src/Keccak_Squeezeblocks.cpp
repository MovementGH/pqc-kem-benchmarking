/**
 * @file Keccak_Squeezeblocks.cpp
 * @brief Implementation of Keccak_Squeezeblocks.h
 */

#include "Keccak_Squeezeblocks.h"

/**
 * @brief Keccak function that permutes the internal state and squeezes 512 bits out
 *
 *
 * @param[out] h Array containting the 512 squeezed bits
 * @param[inout] state Keccak state
 */
void Keccak_Squeezeblocks_512Bits(ap_uint64 h[SHAKE256_512_64BIT_SIZE], ap_uint64 state[PRNG_STATE_64BIT_SIZE]) {
	ap_uint7 i;
	
	KeccakF1600_StatePermute(state);
	Keccak_Squeezeblocks_loop1_1 : for (i = 0; i < SHAKE256_512_64BIT_SIZE; i++) {
		h[i] = state[i];
	}
	state[PRNG_STATE_64BIT_SIZE-1] = SHAKE256_RATE_64BIT_SIZE - SHAKE256_512_64BIT_SIZE;
}


/**
 * @brief Keccak incremental squeeze function
 *
 * Squeezes outputwordsize words of 64 bits out of the internal state. This function will use the state before permuting
 * it if part of the rate is available. It will permute it as many times as needed and will update the state counter to
 * indicate the part of the rate of the final state that has been used for squeezing.
 *
 * @param[out] h Array containting the squeezed bits
 * @param[in] outputwordsize Number of 64 bits blocks that should be squeezed out of the Keccak state
 * @param[inout] state Keccak state
 */
void Keccak_Squeezeblocks_64_Inc(ap_uint64 h[PRNG_MAX_OUTPUT_BYTESIZE], uint8_t outputwordsize, ap_uint64 state[PRNG_STATE_64BIT_SIZE], ap_uint8 r) {
    uint8_t i;
	ap_uint11 offset = 0;
	uint8_t cnt = (ap_uint8)state[PRNG_STATE_64BIT_SIZE - 1];
	uint8_t offset_state = r/8 - cnt;

    /* First consume any bytes we still have sitting around */
    Keccak_Squeezeblocks_64_loop1 : for (i = 0; i < cnt; i++) {
		if (i >= outputwordsize)
			break;
        /* There are state[25] words left, so r/8 - state[25] is the first
           available word. We consume from there, i.e., up to r/8. */
        h[i] = state[offset_state + i];
    }

	offset += i;
    outputwordsize -= i;
    state[PRNG_STATE_64BIT_SIZE - 1] = (ap_uint64)(cnt - i);

    /* Then squeeze the remaining necessary blocks */
	assert(outputwordsize <= 5);
    Keccak_Squeezeblocks_64_loop2 : while (outputwordsize > 0) {
        KeccakF1600_StatePermute(state);

		assert(r == SHAKE256_RATE_BYTESIZE);
        Keccak_Squeezeblocks_64_loop2_1 : for (i = 0; i < r / 8; i++) {
			if (i >= outputwordsize)
				break;
            h[offset + i] = state[i];
        }
 
		offset += i;
        outputwordsize -= i;
        state[PRNG_STATE_64BIT_SIZE - 1] = (ap_uint64)(r / 8 - i);
    }
}

