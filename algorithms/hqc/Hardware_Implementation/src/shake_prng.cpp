#include "shake_prng.h"



/**
 * @brief SHAKE-256 with incremental API and domain separation
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[in] entropy_input Pointer to input entropy bytes
 * @param[in] personalization_string Pointer to the personalization string
 * @param[in] enlen Length of entropy string in bytes
 * @param[in] perlen Length of the personalization string in bytes
 * @param[inout] shake_prng_state_array PRNG state
 */
void shake_prng_init(ap_uint8 entropy_input[SHARED_SECRET_BYTESIZE], ap_uint8 personalization_string[SHARED_SECRET_BYTESIZE], ap_uint32 enlen, ap_uint32 perlen, ap_uint64 shake_prng_state_array[PRNG_STATE_64BIT_SIZE])
{
    ap_uint16 i;
    ap_uint8 domain = PRNG_DOMAIN;
    ap_uint8 temp_buffer[2*SHARED_SECRET_BYTESIZE];
    assert(enlen+perlen <= 2*SHARED_SECRET_BYTESIZE);

    // Build a single buffer to do a single absorption
    memset_1 : for (i = 0; i < enlen; i++){
        temp_buffer[i] = entropy_input[i];
    }
    memset_2 : for (i = 0; i < perlen; i++){
       temp_buffer[enlen+i] = personalization_string[i];
    }

    // Replace shake256_inc_init(&shake_prng_state);
    vector_init_64(shake_prng_state_array, PRNG_STATE_64BIT_SIZE);

    // Replace the following three absorbptions
    //shake256_inc_absorb(&shake_prng_state, entropy_input, enlen);
    //shake256_inc_absorb(&shake_prng_state, personalization_string, perlen);
    //shake256_inc_absorb(&shake_prng_state, , &domain, 1);
    Keccak_Stream_Absorb_With_DS_From_Byte_0(shake_prng_state_array, temp_buffer, NULL, 0, enlen+perlen, domain);

    // Replace the finalize operation
    //shake256_inc_finalize(&shake_prng_state);
    Keccak_Stream_Absorb_Finalize(shake_prng_state_array, 0x1F);
}

/**
 * @brief A SHAKE-256 based PRNG
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[out] output Pointer to output
 * @param[in] outlen length of output in 64 bit words
 * @param[out] state_prng Keccak state
 */
void shake_prng(ap_uint64 output[KEM_SEED_64BIT_SIZE],
                uint8_t outlen,
                ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE])
{
    // Replace shake256_inc_squeeze(output, outlen, &shake_prng_state);
    Keccak_Squeezeblocks_64_Inc(output, outlen, state_prng, SHAKE256_RATE_BYTESIZE);
}


/**
 * @brief Initialiase a SHAKE-256 based seedexpander
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[in] seed A seed
 * @param[in] seedlen The seed length in words of 64 bits
 * @param[out] state Keccak state
 */
void seedexpander_init(ap_uint64 seed[KEM_SEED_64BIT_SIZE], ap_uint16 seedlen, ap_uint64 state[PRNG_STATE_64BIT_SIZE]){
    ap_uint8 j;

    assert(seedlen <= KEM_SEED_64BIT_SIZE);
    // Replace shake256_inc_init(state);
    memset : for (j = 0; j < PRNG_STATE_64BIT_SIZE; ++j) state[j] = 0;
    // Replace shake256_inc_absorb(state, seed, seedlen);
    // Replace shake256_inc_absorb(state, &domain, 1);
    Keccak_Stream_Absorb_With_DS_From_Byte_0(state, NULL, seed, 1, seedlen*sizeof(ap_uint64), SEEDEXPANDER_DOMAIN);
    // Replace shake256_inc_finalize(state);
    Keccak_Stream_Absorb_Finalize(state, 0x1F);
}


/**
 * @brief A SHAKE-256 based seedexpander
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[out] output The XOF data
 * @param[in] outlen Number of vector_byte_type elements to return
 * @param[out] state Keccak state
 */
void seedexpander(vector_byte_type output[VEC_N_BYTESIZE],
                  ap_uint16 outlen,
                  ap_uint64 state[PRNG_STATE_64BIT_SIZE])
{
    ap_uint16 i;
    const uint8_t buffer_size = 1;
    ap_uint64 tmp[buffer_size];
    ap_uint8 j, k;

    assert(outlen <= VEC_N_BYTESIZE);
    seedexpander_main_loop : for (i = 0; i < outlen; i += sizeof(tmp)) {
        Keccak_Squeezeblocks_64_Inc(tmp, buffer_size, state, SHAKE256_RATE_BYTESIZE);
        copystate_seedexpander_final:for (j = 0; j < buffer_size; ++j) {
            seedexpander_shrink_loop: for (k = 0; k < sizeof(uint64_t)/sizeof(vector_byte_type); ++k) {
                if (i + j * 8 + k < outlen) {
                    output[i + j * 8 + k] = (vector_byte_type)(tmp[j] >> (k * 8 * sizeof(vector_byte_type)));
                } // if
            } // for
        } //for
    } // for
}


/**
 * @brief A SHAKE-256 based seedexpander
 *
 * Derived from function SHAKE_256 in fips202.c
 *
 * @param[out] output The XOF data
 * @param[in] outlen Number of vector_word_type elements to return
 * @param[out] state Keccak state
 */
void seedexpander_mult_type(vector_word_type output[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER],
                            ap_uint16 outlen,
                            ap_uint64 state[PRNG_STATE_64BIT_SIZE])
{
    ap_uint16 i;
    ap_uint8 j, k, l;
    const uint8_t buffer_size = 1;
    ap_uint64 tmp[buffer_size];
    ap_uint8 vect_words_in_64bits = sizeof(uint64_t) / VECTOR_WORD_BYTESIZE;


    assert(outlen <= VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER);
    seedexpander_mult_type_main_loop : for (i = 0; i < outlen; i += (sizeof(tmp) / VECTOR_WORD_BYTESIZE)) {
        Keccak_Squeezeblocks_64_Inc(tmp, buffer_size, state, SHAKE256_RATE_BYTESIZE);
        copystate_seedexpander_final:for (j = 0; j < buffer_size; ++j) {
            seedexpander_shrink_loop: for (k = 0; k < vect_words_in_64bits; ++k) {
                if (i + j * vect_words_in_64bits+ k < outlen) {
                     output[i + j * vect_words_in_64bits + k] = (vector_word_type)(tmp[j] >> (k * VECTOR_WORD_BITSIZE));
                } // if
            } // for
        } // for

    } // for

}
