/**
 * @file code.cpp
 * @brief Implementation of concatenated code
 */

#include "code.h"

/**
 *
 * @brief Encoding the message m to a code word em using the concatenated code
 *
 * First we encode the message using the Reed-Solomon code, then with the duplicated Reed-Muller code we obtain
 * a concatenated code word.
 *
 * @param[out] em Array containing the tensor code word
 * @param[in] m Array containing the message
 */
void code_encode(ap_uint8 em[VEC_N1N2_BYTESIZE], ap_uint8 m[VEC_K_BYTESIZE]) {
    ap_uint8 store_rs_encode[VEC_N1_BYTESIZE];

	reed_solomon_encode(store_rs_encode, m);
	reed_muller_encode(em, store_rs_encode);

	#ifdef VERBOSE
		printf("\n\nReed-Solomon code word: "); vect_print((uint8_t *)store_rs_encode, VEC_N1_BYTESIZE);
		printf("\n\nConcatenated code word: "); vect_print((uint8_t *)em, VEC_N1N2_BYTESIZE);
	#endif
}



/**
 * @brief Decoding the code word em to a message m using the concatenated code
 *
 * @param[out] m Array containing the message
 * @param[in] em Array containing the code word
 */
void code_decode(ap_uint8 m[VEC_K_BYTESIZE], ap_uint8 em[VEC_N1N2_BYTESIZE]) {
    ap_uint8 store_rs_decode[VEC_N1_BYTESIZE];

	reed_muller_decode(store_rs_decode, em);
	reed_solomon_decode(m, store_rs_decode);


	#ifdef VERBOSE
		printf("\n\nReed-Muller decoding result (the input for the Reed-Solomon decoding algorithm): ");
		vect_print((uint8_t *)store_rs_decode, VEC_N1_BYTESIZE);
	#endif
}
