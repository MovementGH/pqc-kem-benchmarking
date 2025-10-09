/**
 * @file parsing.cpp
 * @brief Functions to parse secret key, public key and ciphertext of the HQC scheme
 */

#include "parsing.h"

/**
 * @brief Parse a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As technicality, the public key is appended to the secret key in order to respect NIST API.
 * The output Array sk is supposed to already contain the seed in its first coordinates.
 *
 * @param[out] sk String containing the secret key
 * @param[in] pk String containing the public key
 */
void hqc_secret_key_to_string(ap_uint64 sk[SECRET_KEY_BYTESIZE],
                              ap_uint64 pk[PUBLIC_KEY_BYTESIZE]) {
	ap_uint10 i;

    secret_to1:for (i=0; i < PUBLIC_KEY_64BIT_SIZE; ++i) {
        sk[KEM_SEED_64BIT_SIZE + i] = pk[i];
    }
}


/**
 * @brief Parse a secret key from a string
 *
 * The seed, contained in the secret key is used to generate vectors x and y but only y is required by the calling
 * function and returned.
 *
 * @param[out] y uint32_t representation of vector y
 * @param[in] sk String containing the secret key
 * @param[inout] state_seedexpander Internal state of the seed expander
 */
void hqc_secret_key_from_string(vector_index_type y[PARAM_OMEGA],
    ap_uint64 sk[KEM_SEED_64BIT_SIZE],
    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE]) {

	seedexpander_init(sk, KEM_SEED_64BIT_SIZE, state_seedexpander);

    vect_set_random_fixed_weight_by_coordinates(y, PARAM_OMEGA, state_seedexpander); //skip first value which is x's coordinates
	vect_set_random_fixed_weight_by_coordinates(y, PARAM_OMEGA, state_seedexpander);
}


/**
 * @brief Parse a public key from a string
 *
 * The public key is composed of the syndrome <b>s</b> as well as the seed used to generate the vector <b>h</b>.
 * This function retuns as a parameter the syndrome.
 *
 * @param[out] s vector_word_type representation of vector s
 * @param[in] pk String containing the public key
 */
void hqc_public_key_from_string(vector_word_type s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER],
        ap_uint64 pk[PUBLIC_KEY_64BIT_SIZE]) {
    uint16_t i, j;
    uint8_t vect_words_in_64bits = 8*sizeof(uint64_t)/VECTOR_WORD_BITSIZE;

    public_from1:for (i = 0; i < (PUBLIC_KEY_64BIT_SIZE - KEM_SEED_64BIT_SIZE); ++i) {
        pk_shrink_loop: for (j = 0; j < vect_words_in_64bits; ++j) {
            if (i * vect_words_in_64bits + j < VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER ) {
                s[i * vect_words_in_64bits + j] = (vector_word_type)(pk[i + KEM_SEED_64BIT_SIZE] >> (j * VECTOR_WORD_BITSIZE));
            }
        }

    }
	s[VEC_N_VECTORWORDSIZE - 1] &= (vector_word_type)0xff;
}


/**
 * @brief Parse a public key into a string
 *
 * The public key is composed of the syndrome <b>s</b> as well as the seed used to generate the vector <b>h</b>
 * The Seed used to generate the public key is already stored in the string pk
 * @param[out] pk String containing the public key
 * @param[in] s ap_uint8 representation of vector s
 */
void hqc_public_key_to_string(ap_uint64 pk[PUBLIC_KEY_64BIT_SIZE],
                              vector_byte_type s[VEC_N_BYTESIZE]) {
	ap_uint16 i;
    ap_uint4 j;

    public_to1:for (i=0; i<VEC_N_64BIT_SIZE; ++i) {
        pk_expand_loop : for (j =0; j < 8; ++j) {
#ifdef HLS_DATATYPES
			pk[KEM_SEED_64BIT_SIZE+i].range((j * 8) + (vector_byte_type::width - 1), (j * 8)) = s[i * 8 + j ];
#else
            pk[KEM_SEED_64BIT_SIZE+i] |= ((ap_uint64)(s[i * 8 + j ]) << (8 * j)) ;
#endif
        }
    }

}


/**
 * @brief Parse a ciphertext into a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and hash <b>d</b>.
 *
 * @param[in] u ap_uint8 representation of vector u
 * @param[in] v ap_uint8 representation of vector v
 * @param[in] d String containing the hash d
 * @param ct String containing the ciphertext
 */
void hqc_ciphertext_to_string_64(
        vector_byte_type u[VEC_N_BYTESIZE],
        vector_byte_type v[VEC_N1N2_BYTESIZE],
        ap_uint64 d[SHAKE256_512_64BIT_SIZE],
        vector_byte_type ct[CIPHERTEXT_BYTESIZE]) {
	
    ap_uint12 i;
    uint8_t j;

    ct_to1:for (i = 0; i<VEC_N_BYTESIZE; ++i) {
        ct[i] = u[i];
    }

    ct_to2:for (i = 0; i<VEC_N1N2_BYTESIZE; ++i) {
        ct[i + VEC_N_BYTESIZE] = v[i];
    }
    
    ct_to3 : for (i = 0; i < SHAKE256_512_BYTESIZE / 8; i++) {
        ct_to3_shrink_loop: for (j = 0; j < 8; ++j) {
		    ct[i * 8 + j + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE] = (vector_byte_type)((d[i] >> (j * 8)) & 0xff);
        }
	}
}


/**
 * @brief Parse a ciphertext from a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and hash <b>d</b>.
 *
 * @param[out] u ap_uint8 representation of vector u
 * @param[out] v ap_uint8 representation of vector v
 * @param[out] d String containing the hash d
 * @param[in] ct String containing the ciphertext
 */
void hqc_ciphertext_from_string_64(
        vector_byte_type u[VEC_N_BYTESIZE],
        vector_byte_type v[VEC_N1N2_BYTESIZE],
        ap_uint64 d[SHAKE256_512_64BIT_SIZE],
        vector_byte_type ct[CIPHERTEXT_BYTESIZE]) {

	ap_uint13 i, j;

	
    ct_from1:for (i = 0; i<VEC_N_BYTESIZE; ++i) {
        u[i] = ct[i];
    }

	
    ct_from2:for (i = 0; i<VEC_N1N2_BYTESIZE; ++i) {
        v[i] = ct[i + VEC_N_BYTESIZE];
    }

    ct_from3:for (i = 0; i < SHAKE256_512_64BIT_SIZE; ++i) {
        ct_from3_expand_loop: for (j = 0; j < 8; ++j) {
#ifdef HLS_DATATYPES
            d[i].range(j * 8 + 7,j * 8) = ct[i * 8 + j + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE];
#else
		    d[i] |= ((ap_uint64)ct[i * 8 + j + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE]) << (8 * j) ;
#endif
        }
    }
}

