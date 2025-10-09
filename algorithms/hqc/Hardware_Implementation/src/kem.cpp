/**
 * @file kem.cpp
 * @brief NIST KEM API used by the HQC_KEM IND-CCA2 scheme
 */

#include "kem.h"

/**
 * @brief Function grouping a call to keypair, enc, and dec for HLS to implement all functions in hardware
 *
 * This function is here only for HLS.
 * Its goal is to tell HLS to implement a circuit able to compute the three functions keypair, enc and dec.
 * It can also be used to test that for the entropy given as input the decapsulation function returns the expected shared secret.
 *
 * @param[out] ss1 String containing the plaintext shared secret returned by enc
 * @param[out] ss2 String containing the plaintext shared secret returned by dec
 * @param[in] prng_seed seed for the initialization of the PRNG
 * @param[in] seed_bytesize seed size in bytes
 *
 * @returns the return value of the decapsulation function
 */
int crypto_kem_all_functions_hls(ap_uint64 ss1[SHARED_SECRET_64BIT_SIZE],
                                 ap_uint64 ss2[SHARED_SECRET_64BIT_SIZE],
                                 ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                                 ap_uint8 seed_bytesize) {
    static ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE];
    static ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE];
    static ap_uint64 pk64[KEY_MAX_64BIT_SIZE];
    static ap_uint64 sk64[KEY_MAX_64BIT_SIZE];
    static ap_uint8 ct[CIPHERTEXT_BYTESIZE];
    // Global arrays used to store temporary data for KEM
    static ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE];
    static ap_uint64 theta[SHAKE256_512_64BIT_SIZE];
    static ap_uint64 d[SHAKE256_512_64BIT_SIZE];
    // Global arrays used to store temporary data for HQC
    static vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE];
    static vector_byte_type store_B[VEC_N_BYTESIZE];
    static vector_byte_type store_C[VEC_K_BYTESIZE];
    static vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1]; // The extra element is needed to avoid a length check
    static vector_index_type random_vector_A[PARAM_OMEGA_R];
    static vector_index_type random_vector_B[PARAM_OMEGA_R];
    static ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE];

    assert(seed_bytesize <= SHARED_SECRET_BYTESIZE);
    shake_prng_init(prng_seed, NULL, seed_bytesize, 0, state_prng);

    crypto_kem_keypair_internal(pk64, sk64, state_seedexpander, state_prng, mc, theta, store_A, store_B, h_s,
                                random_vector_A, random_vector_B, store_seeds);
    crypto_kem_enc_internal(ct, ss1, pk64, state_seedexpander, state_prng, mc, theta, d, store_A, store_B, store_C,
                            h_s, random_vector_A, random_vector_B, store_seeds);
    return crypto_kem_dec_internal(ss2, ct, sk64, state_seedexpander, mc, theta, d, store_A, store_B, store_C,
                                   h_s, random_vector_A, random_vector_B, store_seeds);
}

/**
 * @brief Keygen of the HQC_KEM IND_CAA2 scheme, HLS version
 *
 * Generates a key pair for HQC_KEM using a PRNG initialised with the seed array given as input.
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 *
 * @param[out] pk String containing the public key
 * @param[out] sk String containing the secret key
 * @param[in] prng_seed seed for the initialization of the PRNG
 * @param[in] seed_bytesize seed size in bytes
 */
void crypto_kem_keypair_hls(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                            ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                            ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                            ap_uint8 seed_bytesize) {
    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE] = {0};
    ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE] = {0};
    ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE] = {0};
    ap_uint64 theta[SHAKE256_512_64BIT_SIZE] = {0};
    vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE] = {0};
    vector_byte_type store_B[VEC_N_BYTESIZE] = {0};
    vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1] = {0}; // The extra element is needed to avoid a length check
    vector_index_type random_vector_A[PARAM_OMEGA_R] = {0};
    vector_index_type random_vector_B[PARAM_OMEGA_R] = {0};
    ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE] = {0};

    assert(seed_bytesize <= SHARED_SECRET_BYTESIZE);
    shake_prng_init(prng_seed, NULL, seed_bytesize, 0, state_prng);
    crypto_kem_keypair_internal(pk, sk, state_seedexpander, state_prng, mc, theta, store_A, store_B, h_s,
                                random_vector_A, random_vector_B, store_seeds);
}

/**
 * @brief Encapsulation function of the HQC_KEM IND_CAA2 scheme, HLS version
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 * @param[out] ct String containing the ciphertext
 * @param[out] ss String containing the shared secret
 * @param[in] pk String containing the public key
 * @param[in] prng_seed seed for the initialization of the PRNG
 * @param[in] seed_bytesize seed size in bytes
 */
void crypto_kem_enc_hls(ap_uint8 ct[CIPHERTEXT_BYTESIZE],
                        ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                        ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                        ap_uint8 prng_seed[SHARED_SECRET_BYTESIZE],
                        ap_uint8 seed_bytesize) {
    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE] = {0};
    ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE] = {0};
    ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE] = {0};
    ap_uint64 theta[SHAKE256_512_64BIT_SIZE] = {0};
    ap_uint64 d[SHAKE256_512_64BIT_SIZE] = {0};
    vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE] = {0};
    vector_byte_type store_B[VEC_N_BYTESIZE] = {0};
    vector_byte_type store_C[VEC_K_BYTESIZE] = {0};
    vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1] = {0}; // The extra element is needed to avoid a length check
    vector_index_type random_vector_A[PARAM_OMEGA_R] = {0};
    vector_index_type random_vector_B[PARAM_OMEGA_R] = {0};
    ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE] = {0};

    assert(seed_bytesize <= SHARED_SECRET_BYTESIZE);
    shake_prng_init(prng_seed, NULL, seed_bytesize, 0, state_prng);
    crypto_kem_enc_internal(ct, ss, pk, state_seedexpander, state_prng, mc, theta, d, store_A, store_B, store_C, h_s,
                            random_vector_A, random_vector_B, store_seeds);
}

/**
 * @brief Decapsulation function of the HQC_KEM IND_CAA2 scheme, HLS version
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 * @param[out] ss String containing the shared secret
 * @param[in] ct String containing the cipĥertext
 * @param[in] sk String containing the secret key
 * @returns 0 if decapsulation is successful, 1 otherwise
 */
int crypto_kem_dec_hls(ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                       ap_uint8 ct[CIPHERTEXT_BYTESIZE],
                       ap_uint64 sk[KEY_MAX_64BIT_SIZE]) {
    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE] = {0};
    // Global arrays used to store temporary data for KEM
    ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE] = {0};
    ap_uint64 theta[SHAKE256_512_64BIT_SIZE] = {0};
    ap_uint64 d[SHAKE256_512_64BIT_SIZE] = {0};
    // Global arrays used to store temporary data for HQC
    vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE] = {0};
    vector_byte_type store_B[VEC_N_BYTESIZE] = {0};
    vector_byte_type store_C[VEC_K_BYTESIZE] = {0};
    vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1] = {0}; // The extra element is needed to avoid a length check
    vector_index_type random_vector_A[PARAM_OMEGA_R] = {0};
    vector_index_type random_vector_B[PARAM_OMEGA_R] = {0};
    ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE] = {0};

    return crypto_kem_dec_internal(ss, ct, sk, state_seedexpander, mc, theta, d, store_A, store_B, store_C,
                                   h_s, random_vector_A, random_vector_B, store_seeds);
}

/**
 * @brief Keygen of the HQC_KEM IND_CAA2 scheme, internal version
 *
 * Generates a key pair for HQC_KEM using a PRNG initialised with the store_seeds array given as input.
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 *
 * @param[out] pk String containing the public key
 * @param[out] sk String containing the secret key
 * @param[inout] state_seedexpander internal state of the seedexpander
 * @param[inout] state_prng internal state of the prng
 * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
 */
void crypto_kem_keypair_internal(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                                 ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                                 ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                                 ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
                                 // Global arrays used to store temporary data for KEM
                                 ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                                 ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                                 // Global arrays used to store temporary data for HQC
                                 vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                                 vector_byte_type store_B[VEC_N_BYTESIZE],
                                 vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1], // The extra element is needed to avoid a length check
                                 vector_index_type random_vector_A[PARAM_OMEGA_R],
                                 vector_index_type random_vector_B[PARAM_OMEGA_R],
                                 ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {
#ifdef VERBOSE
    printf("\n\n\n\n### KEYGEN ###");
#endif

    hqc_pke_keygen(pk, sk, state_seedexpander, state_prng, store_A, store_B, h_s, random_vector_A, random_vector_B,
                   store_seeds);
}

/**
 * @brief Encapsulation function of the HQC_KEM IND_CAA2 scheme, internal version
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 * @param[out] ct String containing the ciphertext
 * @param[out] ss String containing the shared secret
 * @param[in] pk String containing the public key
 * @param[inout] state_seedexpander internal state of the seedexpander
 * @param[inout] state_prng internal state of the prng
 * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
 */
void crypto_kem_enc_internal(ap_uint8 ct[CRYPTO_CIPHERTEXTBYTESIZE],
                             ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                             ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                             ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                             ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
                             // Global arrays used to store temporary data for KEM
                             ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                             ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                             ap_uint64 d[SHAKE256_512_64BIT_SIZE],
                             // Global arrays used to store temporary data for HQC
                             vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                             vector_byte_type store_B[VEC_N_BYTESIZE],
                             vector_byte_type store_C[VEC_K_BYTESIZE],
                             vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1], // The extra element is needed to avoid a length check
                             vector_index_type random_vector_A[PARAM_OMEGA_R],
                             vector_index_type random_vector_B[PARAM_OMEGA_R],
                             ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {
#ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
#endif
    // Computing m
    // Use an intermediate array store_seeds so that all calls to shake_prng have an array of
    // size KEM_SEED_64BIT_SIZE as target. This will allow to factorize the calls in hardware to reduce the footprint.
    vect_set_random_from_prng(mc, store_seeds, state_prng);

    // Computing theta
    shake256_512_ds(theta, mc, VEC_K_BYTESIZE, G_FCT_DOMAIN);

    vect_copyresize(store_C, VEC_K_BYTESIZE, mc, VEC_K_BYTESIZE);

    // Encrypting m
    hqc_pke_encrypt(&mc[VEC_K_BYTESIZE], &mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE], store_C, theta, pk,
                    state_seedexpander, store_A, store_B, h_s, random_vector_A, random_vector_B, store_seeds);

    // Computing d
    shake256_512_ds(d, mc, VEC_K_BYTESIZE, H_FCT_DOMAIN);

    // Computing shared secret
    shake256_512_ds(ss, mc, VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE, K_FCT_DOMAIN);

    // Computing ciphertext
    hqc_ciphertext_to_string_64(&mc[VEC_K_BYTESIZE], &mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE], d, ct);


#ifdef VERBOSE
    printf("\n\npk: "); vect_print((uint8_t *)pk, KEY_MAX_64BIT_SIZE*sizeof(uint64_t));
    printf("\n\nmc: "); vect_print((uint8_t *)mc, VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE);
    printf("\n\ntheta: "); vect_print((uint8_t *)theta, SHAKE256_512_64BIT_SIZE * sizeof(uint64_t));
    printf("\n\nd: "); vect_print((uint8_t *)d, SHAKE256_512_64BIT_SIZE * sizeof(uint64_t));
    printf("\n\nciphertext: "); vect_print((uint8_t *)ct, CRYPTO_CIPHERTEXTBYTESIZE);
    printf("\n\nsecret 1: "); vect_print((uint8_t *)ss, SHARED_SECRET_64BIT_SIZE * sizeof(uint64_t));
#endif
}

/**
 * @brief Decapsulation function of the HQC_KEM IND_CAA2 scheme, internal version
 *
 * The internal version has a large signature that includes many arrays that can be shared by multiple functions to
 * reduce hardware usage, as crypto_kem_all_functions_hls does. In crypto_kem_all_functions_hls the arrays are only
 * defined once and shared between keypair enc and dec internal functions. It thus gives a realistic estimation of the
 * cost of implementing the three functions together.
 *
 * The HLS version is the function implemented in hardware when used standalone. It has a reduced signature as it defines
 * the arrays needed for computation locally and calls the internal function. As it includes the arrays needed for the
 * computation, when implemented it provides a realistic estimation of the cost of a standalone implementation.
 *
 * The API version of this function is the same as the HLS one wrapped to use standard C data types instead of HLS data types.
 *
 * @param[out] ss String containing the shared secret
 * @param[in] ct String containing the cipĥertext
 * @param[in] sk String containing the secret key
 * @param[inout] state_seedexpander internal state of the seedexpander
 * @param[inout] state_prng internal state of the prng
 * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
 * @returns 0 if decapsulation is successful, 1 otherwise
 */
int crypto_kem_dec_internal(ap_uint64 ss[SHARED_SECRET_64BIT_SIZE],
                            ap_uint8 ct[CRYPTO_CIPHERTEXTBYTESIZE],
                            ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                            ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                            // Global arrays used to store temporary data for KEM
                            ap_uint8 mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE],
                            ap_uint64 theta[SHAKE256_512_64BIT_SIZE],
                            ap_uint64 d[SHAKE256_512_64BIT_SIZE],
                            // Global arrays used to store temporary data for HQC
                            vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                            vector_byte_type store_B[VEC_N_BYTESIZE],
                            vector_byte_type store_C[VEC_K_BYTESIZE],
                            vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1], // The extra element is needed to avoid a length check
                            vector_index_type random_vector_A[PARAM_OMEGA_R],
                            vector_index_type random_vector_B[PARAM_OMEGA_R],
                            ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {

#ifdef VERBOSE
    printf("\n\n\n\n### DECAPS ###");
#endif


    static ap_uint8 uv2[VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE];

    uint8_t i;

    bool cmp_d_d2 = false;
    bool cmp_u_u2 = false;
    bool cmp_v_v2 = false;
    bool result;
    const ap_uint64 masks[2] = {0xffffffffffffffffULL,0x0};

    // Retrieving u, v and d from ciphertext
    hqc_ciphertext_from_string_64(&mc[VEC_K_BYTESIZE], &mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE], d, ct);

    // Decryting
    hqc_pke_decrypt(mc, &mc[VEC_K_BYTESIZE], &mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE], sk, state_seedexpander,
                    store_A, store_B, h_s, random_vector_A, store_seeds);

    vect_copyresize(store_C, VEC_K_BYTESIZE, mc, VEC_K_BYTESIZE);

    // Computing theta
    shake256_512_ds(theta, mc, VEC_K_BYTESIZE, G_FCT_DOMAIN);
#ifdef VERBOSE
    printf("\n\ntheta: "); vect_print((uint8_t *)theta, SHAKE256_512_64BIT_SIZE * sizeof(uint64_t));
#endif
    // Encrypting m'
    hqc_pke_encrypt(&uv2[VEC_K_BYTESIZE], &uv2[VEC_K_BYTESIZE + VEC_N_BYTESIZE], store_C, theta,
                    &sk[KEM_SEED_64BIT_SIZE], state_seedexpander, store_A, store_B, h_s, random_vector_A,
                    random_vector_B, store_seeds);

    // Computing d'
    shake256_512_ds(theta, mc, VEC_K_BYTESIZE, H_FCT_DOMAIN);

    // Computing shared secret
    shake256_512_ds(ss, mc, VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE, K_FCT_DOMAIN);

    cmp_u_u2 = vect_compare(&mc[VEC_K_BYTESIZE], &uv2[VEC_K_BYTESIZE], VEC_N_BYTESIZE);
    cmp_v_v2 = vect_compare(&mc[VEC_K_BYTESIZE + VEC_N_BYTESIZE], &uv2[VEC_K_BYTESIZE + VEC_N_BYTESIZE],
                            VEC_N1N2_BYTESIZE);
    cmp_d_d2 = vect_compare_64(d, theta, SHAKE256_512_64BIT_SIZE);

    // Abort if c != c' or d != d'
    result = cmp_u_u2 | cmp_v_v2 | cmp_d_d2;

    control_loop:
    for (i = 0; i < SHARED_SECRET_64BIT_SIZE; i++) {
        ss[i] = masks[result] & ss[i];
    }

#ifdef VERBOSE
    printf("\n\nsk: "); vect_print((uint8_t *)sk, KEY_MAX_64BIT_SIZE*sizeof(uint64_t));
    printf("\n\nciphertext: "); vect_print((uint8_t *)ct, CRYPTO_CIPHERTEXTBYTESIZE);
    printf("\n\nmc: "); vect_print((uint8_t *)mc, VEC_K_BYTESIZE + VEC_N_BYTESIZE + VEC_N1N2_BYTESIZE);
    printf("\n\ntheta: "); vect_print((uint8_t *)theta, SHAKE256_512_64BIT_SIZE * sizeof(uint64_t));
    printf("\n\n\n# Checking Ciphertext- Begin #");
    printf("\n\nu2: "); vect_print((uint8_t *)&uv2[VEC_K_BYTESIZE], VEC_N_BYTESIZE);
    printf("\n\nv2: "); vect_print((uint8_t *)&uv2[VEC_K_BYTESIZE + VEC_N_BYTESIZE], VEC_N1N2_BYTESIZE);
    printf("\n\nd2: "); vect_print((uint8_t *)theta, SHAKE256_512_64BIT_SIZE * sizeof(uint64_t));
    printf("\n\n# Checking Ciphertext - End #\n");
#endif

    return result&1;
}


