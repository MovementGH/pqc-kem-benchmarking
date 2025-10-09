/**
 * @file Keccak_Stream_Absorb.cpp
 * @brief Implementation of Keccak_Stream_Absorb.h
 */

#include "Keccak_Stream_Absorb.h"


/**
 * @brief Keccak function that adds a final marker at the end of the capacity
 *
 *
 * @param[inout] state Keccak state
 * @param[in] p Final marker
 */
void Keccak_Stream_Absorb_Finalize(ap_uint64 state[PRNG_STATE_64BIT_SIZE], ap_uint8 p) {

    ap_uint8 cnt = (ap_uint8)(state[PRNG_STATE_64BIT_SIZE - 1] & 0xff);
	ap_uint64 p_switched;

	switch (cnt & 0x7) {
	case 0:
		p_switched = (ap_uint64)p << 0;
		break;
	case 1:
		p_switched = (ap_uint64)p << 8;
		break;
	case 2:
		p_switched = (ap_uint64)p << 16;
		break;
	case 3:
		p_switched = (ap_uint64)p << 24;
		break;
	case 4:
		p_switched = (ap_uint64)p << 32;
		break;
	case 5:
		p_switched = (ap_uint64)p << 40;
		break;
	case 6:
		p_switched = (ap_uint64)p << 48;
		break;
	case 7:
		p_switched = (ap_uint64)p << 56;
		break;
	}
	state[cnt >> 3] ^= p_switched;
	state[SHAKE256_RATE_64BIT_SIZE - 1] ^= (ap_uint64)0x8000000000000000;
	state[PRNG_STATE_64BIT_SIZE - 1] = 0;
}


/**
 * @brief Keccak function that absorbs on a state that has not absorbed anything before and adds a final domain separation byte
 *
 * @param[inout] state Keccak state
 * @param[in] p Final marker
 */
void Keccak_Stream_Absorb_With_DS_From_Byte_0(ap_uint64 state[PRNG_STATE_64BIT_SIZE],
                                              ap_uint8 input[KECCAK_MAX_MESSAGE_BYTESIZE],
                                              ap_uint64 input64[KECCAK_MAX_MESSAGE_BYTESIZE / 8],
                                              ap_uint1 usewidedata,
                                              ap_uint16 inlen,
                                              dom_type domain) {
    ap_uint5 adr;
    ap_uint9 cnt = 0;
    ap_uint64 m_masked, mask;
    len_type len;
    ap_uint16 i = 0;

    ap_uint64 m_64;
	assert(inlen < 4434);
    sha_loop1 : while (inlen >= i + 8) {
		if (usewidedata){
			m_64 = input64[i/8];
		} else {
			m_64 = (ap_uint64)input[i + 0] |
                   (ap_uint64)input[i + 1] << 8 |
                   (ap_uint64)input[i + 2] << 16 |
                   (ap_uint64)input[i + 3] << 24 |
                   (ap_uint64)input[i + 4] << 32 |
                   (ap_uint64)input[i + 5] << 40 |
                   (ap_uint64)input[i + 6] << 48 |
                   (ap_uint64)input[i + 7] << 56;
		}

		/* START Keccak_Stream_Absorb_64_Aligned(&cnt, state, m_64); */
		adr = (ap_uint5)(cnt >> 3);
		cnt += 8;
		state[adr] ^= m_64;
		if (cnt >= SHAKE256_RATE_BYTESIZE) {
			KeccakF1600_StatePermute(state);
			cnt = 0;
		}
		/* STOP Keccak_Stream_Absorb_64_Aligned(&cnt, state, m_64);*/
		i += 8;
    }

    if (usewidedata){
        m_64 = input64[i/8];
    } else {
        m_64 = (ap_uint64) input[i + 0] |
               (ap_uint64) input[i + 1] << 8 |
               (ap_uint64) input[i + 2] << 16 |
               (ap_uint64) input[i + 3] << 24 |
               (ap_uint64) input[i + 4] << 32 |
               (ap_uint64) input[i + 5] << 40 |
               (ap_uint64) input[i + 6] << 48 |
               (ap_uint64) input[i + 7] << 56;
    }

    /* START Keccak_Stream_Absorb_Aligned_Last_with_domain(&cnt, state, m_64, inlen - i, domain); */
    len = inlen - i;
    assert(len < 8);
    assert(domain < 8);

	switch (len) {
        case 0:
            m_masked = domain;
            break;
        case 1:
            m_masked = (m_64 & 0x00000000000000FF) | (((ap_uint64)domain) << 8);
            break;
        case 2:
            m_masked = (m_64 & 0x000000000000FFFF) | (((ap_uint64)domain) << 16);
            break;
        case 3:
            m_masked = (m_64 & 0x0000000000FFFFFF) | (((ap_uint64)domain) << 24);
            break;
        case 4:
            m_masked = (m_64 & 0x00000000FFFFFFFF) | (((ap_uint64)domain) << 32);
            break;
        case 5:
            m_masked = (m_64 & 0x000000FFFFFFFFFF) | (((ap_uint64)domain) << 40);
            break;
        case 6:
            m_masked = (m_64 & 0x0000FFFFFFFFFFFF) | (((ap_uint64)domain) << 48);
            break;
        case 7:
            m_masked = (m_64 & 0x00FFFFFFFFFFFFFF) | (((ap_uint64)domain) << 56);
            break;
    }
	
    adr = (ap_uint5)(cnt >> 3);
    cnt += len + 1;
    state[adr] ^= m_masked;
    if (cnt >= SHAKE256_RATE_BYTESIZE) {
        KeccakF1600_StatePermute(state);
        cnt = 0;
    }
    /* STOP Keccak_Stream_Absorb_Aligned_Last_with_domain(&cnt, state, m_64, inlen - i, domain); */
    // Last state element contains counter of absorbed bytes
    state[PRNG_STATE_64BIT_SIZE - 1] = (ap_uint64)cnt;
}









