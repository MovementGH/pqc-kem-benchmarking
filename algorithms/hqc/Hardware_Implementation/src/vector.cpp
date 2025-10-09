/**
 * @file vector.cpp
 * @brief Implementation of vectors sampling and some utilities for the HQC scheme
 */

#include "vector.h"



 /**
  * @brief Generates a vector of a given Hamming weight
  *
  * This function generates uniformly at random a binary vector of a Hamming weight equal to the parameter <b>weight</b>.
  * To generate the vector we have to sample uniformly at random values in the interval [0, PARAM_N -1]. Suppose the PARAM_N is equal to \f$ 70853 \f$, to select a position \f$ r\f$ the function works as follow:
  *  1. It makes a call to the seedexpander function to obtain a random number \f$ x\f$ in \f$ [0, 2^{24}[ \f$.
  *  2. Let \f$ t = \lfloor {2^{24} \over 70853} \rfloor \times  70853\f$
  *  3. If \f$ x \geq t\f$, go to 1
  *  4. It return \f$ r = x \mod 70853\f$
  *
  * The parameter \f$ t \f$ is precomputed and it's denoted by UTILS_REJECTION_THRESHOLD (see the file parameters.h).
  *
  * @param[in] v Pointer to an array that contains at most PARAM_OMEGA_R elements
  * @param[in] weight Integer that is the Hamming weight. Must be lower than 2^7
  * @param[inout] state_seedexpander keccak state for the seed expander
  */
void vect_set_random_fixed_weight_by_coordinates(vector_index_type v[PARAM_OMEGA_R],
                                                 vector_weight_type weight,
                                                 ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE])
{
    ap_uint16 random_bytes_size = 3 * weight;
    ap_uint8 rand_bytes[3 * PARAM_OMEGA_R]; // weight is expected to be <= PARAM_OMEGA_R
    random_data_type random_data = 0;
    bool exist;
    ap_uint8 i, k;
    vector_byte_type j;

    memset : for (i = 0; i < 3 * PARAM_OMEGA_R; ++i) {
        rand_bytes[i] = 0;
    }

    assert(weight <= PARAM_OMEGA_R);
    i = 0;
    j = random_bytes_size;
    vect_coord1: while (i < weight) {
        exist = false;
        vect_coord2:do {
            if (j == random_bytes_size) {
                seedexpander(rand_bytes, random_bytes_size, state_seedexpander);
                j = 0;
            }

            random_data = ((random_data_type)rand_bytes[j++]) << 16;
            random_data |= ((random_data_type)rand_bytes[j++]) << 8;
            random_data |= (random_data_type)rand_bytes[j++];
        } while ((random_data >= UTILS_REJECTION_THRESHOLD));

        random_data = random_data % PARAM_N;
            
        k = 0;
        assert(i < PARAM_OMEGA_R);
		vect_coord3:while (k < i) {
            if (v[k] == random_data) {
                exist = true;
            }
            k++;
		}
        if (!exist) {
            v[i] = random_data;
            i++;
        }
    }
}


/**
 * @brief Fills a binary vector v from a list of coordinates
 *
 * This function creates a binary vector v of weight equal to the parameter <b>weight</b> by using the coordinates
 * stored in the parameter <b>fixed_weight_by_coord_vector</b>
 *
 * @param[in] v Pointer to an array that contains at most VEC_N_BYTESIZE elements
 * @param[in] weight an integer Integer that is the Hamming weight of v
 * @param[in] fixed_weight_by_coord_vector pointer to an array which is the support of v
 */

void vect_set_random_fixed_weight(vector_byte_type v[VEC_N_BYTESIZE],
                                  vector_weight_type weight,
                                  vector_index_type fixed_weight_by_coord_vector[PARAM_OMEGA_R]) {
    vector_index_type random_vector_local;
    vector_byte_type v_local;
    vector_size_type index;
    ap_uint3 pos;
    vector_weight_type i;

    vector_init(v, VEC_N_BYTESIZE);

    assert(weight <= PARAM_OMEGA_R);
    vect_weight:for (i = 0; i < weight; i++) {
        random_vector_local = fixed_weight_by_coord_vector[i];
        index = random_vector_local / 8;
        pos = random_vector_local % 8;
        v_local = v[index];
        v[index] = v_local | ((vector_byte_type)1) << pos;
    }
}


/**
 * @brief Generates a random vector of <b>VEC_K_BYTESIZE</b> bytes
 *
 * This function generates a random binary vector of <b>VEC_K_BYTESIZE</b> bytes.
 *
 * @param[in] v Pointer to an array that contains at most VEC_K_BYTESIZE elements
 * @param[in] store_seeds Array used to store the output of the PRNG <b>not a seed</b>, variable re-use to reduce HW resource usage
 * @param[inout] state_prng Pointer to the context of the prng
 */
void vect_set_random_from_prng(vector_byte_type v[VEC_K_BYTESIZE],
                               ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE],
                               ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE]) {
    ap_uint8 i;
    assert(VEC_K_64BIT_SIZE <= KEM_SEED_64BIT_SIZE);
	shake_prng(store_seeds, VEC_K_64BIT_SIZE, state_prng);
	vect_set_random__shrink_loop : for (i = 0; i < VEC_K_BYTESIZE; i++){
        v[i] = (ap_uint8)(store_seeds[i / 8] >> ((i % 8) * 8));
	}
}


/**
 * @brief Generates a random vector of dimension <b>PARAM_N</b>
 *
 * This function generates a random binary vector of dimension <b>PARAM_N</b>. It generates a random
 * array of bytes using the seedexpander function, and drops the extra bits using a mask.
 *
 * @param[in] v Pointer to an array that contains at most VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1 elements
 * @param[in] ctx Pointer to the context of the seed expander
 */
void vect_set_random_mult_type(vector_word_type v[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                               ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE])
{
	ap_uint16 i;
	seedexpander_mult_type(v, (ap_uint16)VEC_N_VECTORWORDSIZE, state_seedexpander);
    v[VEC_N_VECTORWORDSIZE - 1] &= (vector_word_type)(BITMASK(PARAM_N, sizeof(vector_word_type) * 8));
    vect_set_random_mult_type_init_loop : for (i = VEC_N_VECTORWORDSIZE; i < VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1; ++i){
        v[i] = 0;
    }
}


/**
 * @brief Adds two vectors. Every vector must have
 * at most VEC_N_BYTESIZE elements.
 *
 * @param[out] o Pointer to an array that is the result
 * @param[in] v1 Pointer to an array that is the first vector
 * @param[in] v2 Pointer to an array that is the second vector
 * @param[in] size Integer that is the size of the vectors
 */
void vect_add(vector_byte_type o[VEC_N_BYTESIZE], vector_byte_type v1[VEC_N_BYTESIZE], vector_byte_type v2[VEC_N_BYTESIZE], uint16_t size) {
    uint16_t i;
    assert(size<=VEC_N_BYTESIZE);
    vect_addi:for (i = 0 ; i < size; i++) {
		o[i] = v1[i] ^ v2[i];
    }
}


/**
 * @brief Compares two byte vectors of VEC_N_BYTESIZE elements at most.
 *
 * @param[in] v1 Pointer to an array that is first vector
 * @param[in] v2 Pointer to an array that is second vector
 * @param[in] size Integer that is the size of the vectors
 * @returns 0 if the vectors are equal and a negative/positive value otherwise
 */
bool vect_compare(vector_byte_type v1[VEC_N_BYTESIZE], vector_byte_type v2[VEC_N_BYTESIZE], vector_size_type size) {
    bool diff;
    vector_size_type i;

    assert(size <= VEC_N_BYTESIZE);
    diff = false;
    vect_comp:for (i = 0; i < size; i++) {
		diff |= (v1[i] != v2[i]);
	}
    return diff;
}


/**
 * @brief Compares two 64-bit vectors of SHAKE256_512_64BIT_SIZE elements at most.
 *
 * @param[in] v1 Pointer to an array that is first vector
 * @param[in] v2 Pointer to an array that is second vector
 * @param[in] size Integer that is the size of the vectors
 * @returns 0 if the vectors are equal and a negative/positive value otherwise
 */
bool vect_compare_64(ap_uint64 v1[SHAKE256_512_64BIT_SIZE], ap_uint64 v2[SHAKE256_512_64BIT_SIZE], vector_size_type size) {

    bool diff;
    vector_size_type i;

    assert(size <= SHAKE256_512_64BIT_SIZE);
    diff = false;
    vect_comp:for (i = 0; i < size; i++) {
		diff |= (v1[i] != v2[i]);
	}
    return diff;
}


/**
 * @brief Fills with zeros a byte vector of size at most VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE
 *
 * @param[in] array Pointer to an array to fill with zeros
 * @param[in] size Integer that is the size of the array 
 */
void vector_init(vector_byte_type array[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE], uint16_t size)
{
    uint16_t i;
    assert(size <= VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE);
    vector_init_vartype_loop: for(i = 0; i < size; ++i){
        array[i] = 0;
    }
}


/**
 * @brief Fills with zeros a 64-bit vector of size at most SECRET_KEY_64BIT_SIZE
 *
 * @param[in] array Pointer to an array to fill with zeros
 * @param[in] size Integer that is the size of the array 
 */

void vector_init_64(ap_uint64 array[SECRET_KEY_64BIT_SIZE], uint16_t size)
{
    uint16_t i;
    assert(size <= SECRET_KEY_64BIT_SIZE);
    vector_init_kem_uint64_loop: for(i = 0; i < size; ++i){
        array[i] = 0;
    }
}


/**
 * @brief Copy and resize a vector so that it contains <b>size_o</b> bytes.
 *
 * @param[out] o Pointer to the output vector
 * @param[in] size_o Integer that is the size of the output vector in bytes
 * @param[in] v Pointer to the input vector
 * @param[in] size_v Integer that is the size of the input vector in bytes
 */
void vect_copyresize(vector_byte_type o[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], uint16_t size_o, const vector_byte_type v[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE], uint16_t size_v) {
    uint16_t i;
    assert(size_o <= VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE && size_v <= VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE);

#if size_o <= size_v 
        vect_rze_1:for (i=0; i<size_o; ++i) {
            o[i]=v[i];
        }
#else
        vect_rze_1:for (i=0; i<size_v; ++i) {
            o[i]=v[i];
        }
        vect_rze_2:for (i=size_v; i<size_o; ++i) {
            o[i] = 0;
        }
#endif
}

/**
 * @brief Copy and resize a 64-bit vector so that it contains <b>size_o</b> 64-bit words.
 *
 * @param[out] o Pointer to the output vector
 * @param[in] size_o Integer that is the size of the output vector in words
 * @param[in] v Pointer to the input vector
 * @param[in] size_v Integer that is the size of the input vector in words
 */

void vect_copyresize_64(ap_uint64 o[KEY_MAX_64BIT_SIZE], uint8_t size_o, const ap_uint64 v[KEY_MAX_64BIT_SIZE], uint8_t size_v) {
    uint8_t i;

#if size_o <= size_v 
        assert(size_o <= KEY_MAX_64BIT_SIZE);
        vect_rze_1:for (i=0; i<size_o; ++i) {
            o[i]=v[i];
        }
#else
        assert(size_v <= KEY_MAX_64BIT_SIZE);
        vect_rze_1:for (i=0; i<size_v; ++i) {
            o[i]=v[i];
        }
        assert(size_o <= KEY_MAX_64BIT_SIZE);
        vect_rze_2:for (i=size_v; i<size_o; ++i) {
            o[i] = 0;
        }
#endif
}

/**
 * @brief Copy and resize a vector so that it contains <b>VEC_N_VECTORWORDSIZE_FOR_MULTPLIER</b> elements of type vector_word_type.
 *
 * @param[out] o Pointer to the output word vector
 * @param[in] v Pointer to the input byte vector
 */
void vect_copyresize_mult(vector_word_type o[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER], const vector_byte_type v[VEC_N_BYTESIZE]) {
    uint16_t i, j;

	vect_rze_1:for (i = 0; i < VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER; ++i) {
#ifdef HLS_DATATYPES
        expand_loop : for (j =0; j < VECTOR_WORD_BYTESIZE; ++j) {
            if (i*VECTOR_WORD_BYTESIZE+j >= VEC_N_BYTESIZE){
                o[i].range((j * 8) + (vector_byte_type::width - 1), (j * 8)) = 0;
            } else{
                o[i].range((j * 8) + (vector_byte_type::width - 1), (j * 8)) = v[i * VECTOR_WORD_BYTESIZE + j ];
            }
        }
#else
        o[i] = 0;
        expand_loop : for (j =0; j < VECTOR_WORD_BYTESIZE; ++j) {
            if (i*VECTOR_WORD_BYTESIZE+j >= VEC_N_BYTESIZE){break;}
            o[i] |= ((multiplier_word_type)(v[i * VECTOR_WORD_BYTESIZE + j ]) << (8 * j)) ;
        }
#endif
    }
}


#ifdef VERBOSE
/**
 * @brief Prints a given number of bytes
 *
 * @param[in] v Pointer to an array of bytes
 * @param[in] size Integer that is number of bytes to be displayed
 */
void vect_print(const uint8_t *v, const uint32_t size) {
    for (uint32_t i = 0; i < size-1; ++i) {
        printf("%02x", v[i]);
    }
    printf("%02x\n", v[size-1]);
}



/**
 * @brief Prints a vector stored by positions
 *
 * @param[in] v Pointer to an array of integers
 * @param[in] weight Integer that is number positions to be displayed
 */
void vect_print_sparse(const uint16_t *v, const uint16_t weight) {
    for (uint16_t i = 0; i < weight-1; ++i) {
        printf("%d ,", v[i]);
    }
    printf("%d\n", v[weight - 1]);
}

#endif
