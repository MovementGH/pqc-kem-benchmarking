/**
 * @file reed_muller.cpp
 * Constant time implementation of functions around the Reed-Muller code RM(1,7)
 */

#include "reed_muller.h"


/**
 * @brief Encode a single byte into a single codeword using RM(1,7)
 *
 * Encoding matrix of this code: bit pattern
 * 0   aaaaaaaa aaaaaaaa aaaaaaaa aaaaaaaa
 * 1   cccccccc cccccccc cccccccc cccccccc
 * 2   f0f0f0f0 f0f0f0f0 f0f0f0f0 f0f0f0f0
 * 3   ff00ff00 ff00ff00 ff00ff00 ff00ff00
 * 4   ffff0000 ffff0000 ffff0000 ffff0000
 * 5   ffffffff 00000000 ffffffff 00000000
 * 6   ffffffff ffffffff 00000000 00000000
 * 7   ffffffff ffffffff ffffffff ffffffff
 *
 * Result :
 *     w15..... ........ w7w6w5w4  w3w2w1w0
 *
 * @param[out] word An RM(1,7) codeword
 * @param[in] message A ony byte message which will be multiplied as a bit vector with the matrix to produce the output
 */
void encode_8bits(ap_uint8 word[RM_BYTESIZE], vector_byte_type message) {
    
    const ap_uint4 word_adress[18] = {0, 0, 0, 1, 3, 2, 6, 4, 5, 7, 15, 14, 12, 13, 9, 11, 10, 8};
    const ap_uint4 shift_message[18] = {0, 1, 2, 3, 4, 3, 5, 4, 3, 4, 6, 3, 4, 3, 5, 4, 3, 4};
    const ap_uint8 mask[18] = {0xaa, 0xcc, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    vector_byte_type first_word;

    // iterator
    ap_uint5 i;

    first_word = BIT0MASK_8BITS(message>>7);    

    encode_loop: for (i = 0; i < 18; ++i) {
        first_word ^= (vector_byte_type)(BIT0MASK_8BITS(message >> shift_message[i]) & mask[i]);
        word[word_adress[i]] = first_word;
    }
    return;
}


/**
 * @brief Hadamard transform
 *
 * Perform hadamard transform of src and store result in dst
 * src is overwritten: it is also used as intermediate buffer
 * Method is best explained if we use H(3) instead of H(7):
 *
 * The routine multiplies by the matrix H(3):
 *                     [1  1  1  1  1  1  1  1]
 *                     [1 -1  1 -1  1 -1  1 -1]
 *                     [1  1 -1 -1  1  1 -1 -1]
 * [a b c d e f g h] * [1 -1 -1  1  1 -1 -1  1] = result of routine
 *                     [1  1  1  1 -1 -1 -1 -1]
 *                     [1 -1  1 -1 -1  1 -1  1]
 *                     [1  1 -1 -1 -1 -1  1  1]
 *                     [1 -1 -1  1 -1  1  1 -1]
 * You can do this in three passes, where each pass does this:
 * set lower half of buffer to pairwise sums,
 * and upper half to differences
 * index     0        1        2        3        4        5        6        7
 * input:    a,       b,       c,       d,       e,       f,       g,       h
 * pass 1:   a+b,     c+d,     e+f,     g+h,     a-b,     c-d,     e-f,     g-h
 * pass 2:   a+b+c+d, e+f+g+h, a-b+c-d, e-f+g-h, a+b-c-d, e+f-g-h, a-b-c+d, e-f-g+h
 * pass 3:   a+b+c+d+e+f+g+h   a+b-c-d+e+f-g-h   a+b+c+d-e-f-g-h   a+b-c-d-e+-f+g+h
 *                    a-b+c-d+e-f+g-h   a-b-c+d+e-f-g+h   a-b+c-d-e+f-g+h   a-b-c+d-e+f+g-h
 * Likewise, this routine multiplies by H(7) in seven passes.
 *
 * @param[inout] src Structure that contains the initial expanded codeword
 * @param[out] dst Structure that contains the output expanded codeword
 */
void hadamard(ap_int10 src[RM_BITSIZE], ap_int10 dst[RM_BITSIZE]) {
    ap_uint4 pass;

    hadamard1:for (pass = 0; pass < 3; ++pass) {
        hadamard_loop(src, dst);
        hadamard_loop(dst, src);
    }
    hadamard_loop(src, dst);
}


/**
 * @brief Internal loop function for the Hadamard transform
 *
 * @param[in] src input expanded codeword
 * @param[out] dst output expanded codeword
 */
void hadamard_loop(ap_int10 src[RM_BITSIZE], ap_int10 dst[RM_BITSIZE]) {
    ap_uint7 i;
    ap_int10 src_0, src_1;

    hadamard_loop1:for (i = 0; i < RM_BITSIZE/2; i++) {
        src_0 = src[2 * i];
        src_1 = src[2 * i + 1];
        dst[i] = src_0 + src_1;
        dst[i + RM_BITSIZE/2] = src_0 - src_1;
    }
}


/**
 * @brief Add multiple codewords into an expanded codeword
 *
 * Note: this does not write the codewords as -1 or +1 as the green machine does
 * instead, just 0 and 1 is used.
 * The resulting hadamard transform has:
 * all values are halved and the first entry is RM_BITSIZE/2 too high
 *
 * @param[out] dst Structure that contains the expanded codeword
 * @param[in] src Structure that contains the codeword
 */
void expand_and_sum(ap_int10 dst[RM_BITSIZE], vector_byte_type src[VEC_N1N2_BYTESIZE], const ap_uint15 offset) {
    ap_uint5 part, copies;
    ap_uint4 bit;

    exp_sum1: for (copies = 0; copies < NB_OF_RMS; ++copies) {
        exp_sum2: for (part = 0; part < RM_BYTESIZE; part++) {
            exp_sum3: for (bit = 0; bit < 8; bit++) {
                if (copies == 0)
                    dst[part * 8 + bit]  = src[offset + RM_BYTESIZE*copies + part] >> bit & 1;
                else
                    dst[part * 8 + bit] += src[offset + RM_BYTESIZE*copies + part] >> bit & 1;
            }
        }
    }
}


/**
 * @brief Finds the location of the highest value in the input vector
 *
 * This is the final step of the green machine: find the location of the highest value,
 * and add RM_BITSIZE to it if the peak is positive
 * if there are two identical peaks, the peak with smallest value in the lowest 7 bits is taken
 * @param[in] transform Array containing the expanded codeword transformed by the hadamard function
 *
 * @returns the peak position
 */
vector_byte_type find_peaks(ap_int10 transform[RM_BITSIZE]) {
	ap_int10 peak_abs_value = 0;
	ap_int10 peak_value = 0;
	ap_int10 peak_pos = 0;
	ap_int10 new_peak_mask;
	const ap_int10 masks[2] = {0,-1};
    ap_uint8 i;
    find_peak:for (i = 0 ; i < RM_BITSIZE ; ++i) {
		// get absolute value
		ap_int10 t = transform[i];
		ap_int10 absolute = t ^ masks[t<0];
		// define new peak if needed
	    new_peak_mask = masks[absolute > peak_abs_value];
		peak_value ^= new_peak_mask & (peak_value^t);
		peak_pos ^= new_peak_mask & ((ap_int10)i^peak_pos);
		peak_abs_value ^= new_peak_mask & (absolute^peak_abs_value);
	}
	// set bit 7
	peak_pos |= (ap_int10)(RM_BITSIZE * (peak_value > 0));
	return (vector_byte_type)peak_pos;
}


/**
 * @brief Encodes the received word
 *
 * The message is an RS codeword of N1 bytes. Each byte will be encoded into NB_OF_RMS RM codewords.
 *
 * @param[out] cdw Array of size VEC_N1N2_BYTESIZE receiving the RM-RS codeword
 * @param[in] msg Array of size VEC_N1_BYTESIZE with the initial RS codeword
 */
void reed_muller_encode(vector_byte_type cdw[VEC_N1N2_BYTESIZE], vector_byte_type msg[VEC_N1_BYTESIZE]) {
    ap_uint6 i;
    ap_uint5 j,k;
    uint16_t adr1, adr2;

    rm_encode1: for (i = 0 ; i < VEC_N1_BYTESIZE ; i++) {
		// encode first word
		encode_8bits(cdw + (RM_BYTESIZE*NB_OF_RMS)*i, msg[i]);
        // copy to other identical codewords
        rm_encode2: for (k = 0; k < NB_OF_RMS; ++k) {
		    rm_encode3: for (j = 0; j < RM_BYTESIZE; ++j){
                adr1 = (RM_BYTESIZE*NB_OF_RMS)*i + RM_BYTESIZE*k + j;
                adr2 = (RM_BYTESIZE*NB_OF_RMS)*i + j;
                cdw[adr1] = cdw[adr2];
		    }
        }
	}
	return;
}


/**
 * @brief Decodes the received word
 *
 * Decoding uses fast hadamard transform, for a more complete picture on Reed-Muller decoding, see MacWilliams, Florence Jessie, and Neil James Alexander Sloane.
 * The theory of error-correcting codes @cite macwilliams1977theory. The input is a tensor code with an RS and an RM layer.
 * This function decodes the RM layer and returns an RS codeword
 *
 * @param[out] msg Array of size VEC_N1_BYTESIZE receiving the decoded RS codeword
 * @param[in] cdw Array of size VEC_N1N2_BYTESIZE storing the received RM-RS word
 */
void reed_muller_decode(vector_byte_type msg[VEC_N1_BYTESIZE], vector_byte_type cdw[VEC_N1N2_BYTESIZE]) {
    ap_int10 expanded[RM_BITSIZE], transform[RM_BITSIZE];
    uint16_t i;
    rm_decode:for (i = 0; i < VEC_N1_BYTESIZE; ++i) {
        // collect the codewords
        expand_and_sum(expanded, cdw, (RM_BYTESIZE*NB_OF_RMS) * i);
        // apply hadamard transform
        hadamard(expanded, transform);
        // fix the first entry to get the half Hadamard transform
        transform[0] -= RM_BITSIZE*NB_OF_RMS/2;
        // finish the decoding
        msg[i] = find_peaks(transform);
    }
}
