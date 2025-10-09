/**
 * \file gf2x.cpp
 * \brief Polynomial multiplication implementation
 */

#include "gf2x.h"



 /**
  * @brief Compute o(x) = (a1(x) mod \f$ X^n - 1\f$) + a2(x)
  *
  * This function computes the modular reduction of the polynomial a1(x) and adds a2(x) to the result
  *
  * @param[out] o Array containing the result
  * @param[in] a1 Array containing the polynomial to reduce
  * @param[in] a2 Array containing the polynomial to add
  */
void reduce_naive_add(vector_byte_type o[VEC_N_BYTESIZE], multiplier_word_type a1[VEC_N_MULTIPLIERWORDSIZE << 1], vector_byte_type a2[VEC_N_BYTESIZE]) {
    multiplier_word_type r, a_xor_r;
    multiplier_word_type carry;
    const ap_uint12 dec64 = PARAM_N & (MULTIPLIER_BITSIZE - 1);
    const ap_uint12 i64 = PARAM_N / MULTIPLIER_BITSIZE;
    const ap_uint12 d0 = MULTIPLIER_BITSIZE - (PARAM_N & (MULTIPLIER_BITSIZE - 1));
    ap_uint14 i;
    ap_uint7 j;
    ap_uint13 adr;

    reduce_1:for (i = 0; i < i64 + 1; i++) {
        r = a1[i + i64] >> dec64;
        carry = a1[i + i64 + 1] << d0;
        r ^= carry;
        a_xor_r = a1[i] ^ r;
        a_shrink_loop : for (j =0; j < MULTIPLIER_BYTESIZE; ++j) {
            adr = i * MULTIPLIER_BYTESIZE + j;
            if  (adr < VEC_N_BYTESIZE) {
#ifdef HLS_DATATYPES
            o[adr] = (a_xor_r.range(j * 8 + 7, j * 8)) ^ a2[adr];
#else 
            o[adr] = ((ap_uint8)(a_xor_r >> (j  * 8))) ^ a2[adr];
#endif
            }
        }

    }
    o[PARAM_N >> 3] &= (vector_byte_type)RED_MASK;
}

/**
 * @brief Multiply two polynomials.
 *
 * This functions multiplies a sparse polynomial <b>a1</b> (of Hamming weight equal to <b>weight</b>)
 * and a dense polynomial <b>a2</b>. The result and input dense polynomial are arrays of multiplier_word_type
 * variables whose width can be defined at compilation. Increasing this width reduces the time needed for this
 * operation at the expense of increasing the hardware resources used.
 *
 * @param[out] o Array with the result
 * @param[in] a1 Array with a sparse polynomial
 * @param[in] a2 Array with a dense polynomial
 * @param[in] weight Integer containing the weight of the sparse polynomial
 */
void naive_convolution_mult(multiplier_word_type o[VEC_N_MULTIPLIERWORDSIZE << 1], vector_index_type a1[PARAM_OMEGA_R], multiplier_word_type a2[VEC_N_MULTIPLIERWORDSIZE + 1], ap_uint7 weight) {
    vector_index_type d_init, a1_local, dl;
    ap_uint7 k;

#ifndef HLS_DATATYPES
    multiplier_word_type a2_previous, a2_local, a2_shifted;
    ap_uint13 r, r_neg;
    multiplier_word_type o_local;
#else
    multiplier_dw_type a2_local;
    unsigned int r, r_neg;
#endif

    assert(weight <= PARAM_OMEGA_R);
    naive_convolution_mult3:for (k = 0; k < weight; ++k) {
        a1_local = a1[k];
        d_init = a1_local / MULTIPLIER_BITSIZE;
        r = a1_local % MULTIPLIER_BITSIZE;
        r_neg = MULTIPLIER_BITSIZE - r;
        a2_local = 0;
        naive_convolution_mult1:for (dl = 0; dl < VEC_N_MULTIPLIERWORDSIZE + 1; ++dl) {
#ifndef HLS_DATATYPES
            a2_previous = a2_local;
            o_local = o[d_init + dl];
            a2_local = a2[dl];
            // This would leak the value of r_neg but it is not used in the hardware implementation which needs HLS_DATATYPES.
            // This block of code is only used to test that the same results are obtained with C types and HLS types.
            if (r_neg == MULTIPLIER_BITSIZE)
                a2_shifted = 0;
            else
                a2_shifted = a2_previous >> r_neg;

            o_local ^= a2_shifted | (a2_local << r);
            o[d_init + dl] = o_local;
#else
            a2_local = (a2[dl], (multiplier_word_type)(a2_local >> MULTIPLIER_BITSIZE));
            o[d_init + dl] ^= (multiplier_word_type)(a2_local >> r_neg);
#endif
       
        }
    }
}

/**
 * @brief Multiply and add three polynomials modulo \f$ X^n - 1\f$.
 *
 * This functions multiplies a sparse polynomial <b>a1</b> (of Hamming weight equal to <b>weight</b>)
 * and a dense polynomial <b>a2</b>. The multiplication is done modulo \f$ X^n - 1\f$.
 * It then adds to the result a third polynomial a3.
 *
 * @param[out] o Array with the result
 * @param[in] a1 Array with the sparse polynomial
 * @param[in] a2 Array with the dense polynomial
 * @param[in] weight Integer containing the weight of the sparse polynomial
 * @param[in] a3 Array with the polynomial to add
 */
void vect_mul_add(vector_byte_type o[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], vector_index_type a1[PARAM_OMEGA_R], vector_word_type a2[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1], ap_uint7 weight, vector_byte_type a3[VEC_N_BYTESIZE]) {

    multiplier_word_type a2_large[VEC_N_MULTIPLIERWORDSIZE + 1];
    multiplier_word_type res_mult[VEC_N_MULTIPLIERWORDSIZE << 1];
    ap_uint14 i;
    ap_uint5 j;
    
    vector_init_res_mult(res_mult);

#if MULTIPLIER_BITSIZE == VECTOR_WORD_BITSIZE
    a2[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER] = 0;
    naive_convolution_mult(res_mult, a1, a2, weight);
#else

#ifndef HLS_DATATYPES
    memset_init_a2_large: for(i = 0; i < VEC_N_MULTIPLIERWORDSIZE+1 ; ++i){
        a2_large[i] = 0;
    }
#endif

    store_loop : for (i = 0; i < VEC_N_MULTIPLIERWORDSIZE; ++i) {
        a2_expand_loop : for (j = 0; j < MULTIPLIER_VECTORWORDSIZE; ++j) {
#ifdef HLS_DATATYPES
			a2_large[i].range((VECTOR_WORD_BITSIZE * j) + (VECTOR_WORD_BITSIZE - 1), (VECTOR_WORD_BITSIZE * j)) = a2[i * MULTIPLIER_VECTORWORDSIZE + j ] ;
#else
        	a2_large[i] |= (multiplier_word_type)a2[i * MULTIPLIER_VECTORWORDSIZE + j ] << VECTOR_WORD_BITSIZE * j ;
#endif
        }
    }

    a2_large[VEC_N_MULTIPLIERWORDSIZE] = 0;
    naive_convolution_mult(res_mult, a1, a2_large, weight);
#endif // MULTIPLIER_BITSIZE != VECTOR_WORD_BITSIZE
    
    reduce_naive_add(o, res_mult, a3);
}

/**
 * @brief Initialize to 0 the result var of vec_mul_add
 *
 * This functions takes as input an array which is expected to be of type multiplier_word_type and size
 * VEC_N_MULTIPLIERWORDSIZE*2. It initialises its coordinates to 0.
 *
 * @param[out] o Array to be initialized to 0
 */

void vector_init_res_mult(multiplier_word_type array[VEC_N_MULTIPLIERWORDSIZE << 1]) {
    ap_uint14 i;

    memset_init_mult: for(i = 0; i < VEC_N_MULTIPLIERWORDSIZE << 1 ; ++i){
        array[i] = 0;
    }
}





