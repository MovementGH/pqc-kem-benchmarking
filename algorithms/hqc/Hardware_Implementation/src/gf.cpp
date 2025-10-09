/**
 * @file gf.cpp
 * Galois field implementation
 */
#include "gf.h"

/**
 * Multiplies the polynomial representations of two elements 'a' and 'b' in GF(2^8) without reducing the result
 * @returns the product polynomial a*b unreduced
 * @param[in] a First element of GF(2^8) to multiply
 * @param[in] b Second element of GF(2^8) to multiply
 */
gf_dw_type gf_clmul(gf_word_type a, gf_word_type b) {
    ap_uint4 i;
    gf_dw_type c = 0;
    gf_dw_type b2 = b;
    const gf_dw_type masks[2] = {0x0, 0xffff};

    multstep:for(i = 0; i < 8; ++i) {
        // Without cache timing attacks it is simple to do conditional attributions with a multiplexed array of masks
        c ^= b2 & masks[a&1];
        a >>= 1;
        b2 <<= 1;
    }

    return c;
}


/**
 * Multiplies two elements 'a' and 'b' in GF(2^8)
 * @returns the product polynomial a*b reduced
 * @param[in] a First element of GF(2^8) to multiply
 * @param[in] b Second element of GF(2^8) to multiply
 */
gf_word_type gf_mul(gf_word_type a, gf_word_type b) {
    return gf_reduce(gf_clmul(a, b));
}



/**
 * Squares an element of GF(2^8)
 * @returns a^2
 * @param[in] a Element of GF(2^8)
 */
gf_word_type gf_square(gf_word_type a) {
    gf_dw_type b = a;
    gf_dw_type s = b & 1;
    ap_uint4 i;

    squarestep:for(i = 1; i < 8; ++i) {
        b <<= 1;
        s ^= b & (gf_dw_type)(1 << 2 * i);
    }

    return gf_reduce(s);
}


/**
 * Computes the inverse of an element of GF(2^8).
 * @returns the inverse of a
 * @param[in] a Element of GF(2^8)
 */
gf_word_type gf_inverse(gf_word_type a) {
    gf_word_type inv = 1;
    ap_uint4 i;
    gf_inverse_loop: for (i = 0; i < 7; ++i) {
        a = gf_square(a);
        inv = gf_mul(inv, a);
    }

    return inv;
}

/**
 * Reduces the input polynomial modulo the quotient polynomial used to build GF(2^8)
 * @returns The input polynomial reduced to be an element of GF(2^8)
 * @param[in] i The polynomial to be reduced as a double word
 */
gf_word_type gf_reduce(gf_dw_type x) {
    ap_uint2 i;
    gf_dw_type mod;

    reductionstep:for (i = 0; i < 2; ++i) {
        mod = x >> 8;
        x &= (gf_dw_type)0xff;
        x ^= mod;
        mod <<= 2;
        x ^= mod;
        mod <<= 1;
        x ^= mod;
        mod <<= 1;
        x ^= mod;
    }

    return x;
}
