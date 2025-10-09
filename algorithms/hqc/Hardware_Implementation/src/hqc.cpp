/**
 * @file hqc.cpp
 * @brief Implementation of hqc.h
 */

#include "hqc.h"


/**
  * @brief Keygen of the HQC_PKE IND_CPA scheme
  *
  * The public key is composed of syndrome <b>s</b> as well as a <b>seed</b> used to generate the vector <b>h_s</b>.
  *
  * The secret key is composed of the <b>seed</b> used to generate vectors <b>x</b> and <b>y</b>.
  * As a technicality, the public key is appended to the secret key in order to respect NIST API.
  *
  * @param[out] pk String containing the public key
  * @param[out] sk String containing the secret key
  * @param[inout] state_seedexpander internal state of the seedexpander
  * @param[inout] state_prng internal state of the prng
  * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
  */
void hqc_pke_keygen(ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                    ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                    ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                    ap_uint64 state_prng[PRNG_STATE_64BIT_SIZE],
	                // Global arrays used to store temporary data
	                vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                    vector_byte_type store_B[VEC_N_BYTESIZE],
                    vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                    vector_index_type random_vector_A[PARAM_OMEGA_R],
                    vector_index_type random_vector_B[PARAM_OMEGA_R],
                    ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {
    // Iterator
	unsigned int i;

	// Generate seed for secret key
	// Use an intermediate array store_seeds so that all calls to shake_prng and seedexpander_init have an array of
	// size KEM_SEED_64BIT_SIZE as target. This will allow to factorize the calls in hardware to reduce the footprint.
    shake_prng(store_seeds, KEM_SEED_64BIT_SIZE, state_prng);
	seedexpander_init(store_seeds, KEM_SEED_64BIT_SIZE, state_seedexpander);
    vect_copyresize_64(sk, KEM_SEED_64BIT_SIZE, store_seeds, KEM_SEED_64BIT_SIZE);

#ifdef VERBOSE
	printf("\n\nsk_seed: "); vect_print((uint8_t *)store_seeds, KEM_SEED_64BIT_SIZE * sizeof(ap_uint64));
#endif
	// Compute secret key
	// Generate x and store it in store_B
    vect_set_random_fixed_weight_by_coordinates(random_vector_A, PARAM_OMEGA, state_seedexpander);
	vect_set_random_fixed_weight(store_B, PARAM_OMEGA, random_vector_A);

	// Generate y and store it in random_vector_B by coordinates
	vect_set_random_fixed_weight_by_coordinates(random_vector_B, PARAM_OMEGA, state_seedexpander);

    // Generate seed for public key
    // As above use an intermediate array to reduce hardware footprint
	shake_prng(store_seeds, KEM_SEED_64BIT_SIZE, state_prng);
	seedexpander_init(store_seeds, KEM_SEED_64BIT_SIZE, state_seedexpander);
    vect_copyresize_64(pk, KEM_SEED_64BIT_SIZE, store_seeds, KEM_SEED_64BIT_SIZE);

	// Compute public key
	// Generate h_s
	vect_set_random_mult_type(h_s, state_seedexpander);
	// Store public key h_s * random_vector_B + store_B = h_s*y + x into store_A
	vect_mul_add(store_A, random_vector_B, h_s, PARAM_OMEGA, store_B);

	// Parse keys to string
	hqc_public_key_to_string(pk, store_A);
	hqc_secret_key_to_string(sk, pk);

#ifdef VERBOSE
	printf("\n\nx: "); vect_print((uint8_t *)store_B, VEC_N_BYTESIZE);
	printf("\n\ny: "); vect_print_sparse((uint16_t *)random_vector_A, PARAM_OMEGA);

	printf("\n\npk_seed: "); vect_print((uint8_t *)store_seeds, KEM_SEED_64BIT_SIZE * sizeof(ap_uint64));
	printf("\n\nh_s: "); vect_print((uint8_t *)h_s, VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER*VECTOR_WORD_BYTESIZE);

	printf("\n\nsk: "); vect_print((uint8_t *)sk, KEY_MAX_64BIT_SIZE*sizeof(ap_uint64));
	printf("\n\npk: "); vect_print((uint8_t *)pk, KEY_MAX_64BIT_SIZE*sizeof(ap_uint64));
#endif
}

/**
 * @brief Encryption of the HQC_PKE IND_CPA scheme
 *
 * The cihertext is composed of vectors <b>u</b> and <b>v</b>.
 *
 * @param[out] u Vector u (first part of the ciphertext)
 * @param[out] v Vector v (second part of the ciphertext)
 * @param[in] m Vector representing the message to encrypt
 * @param[in] theta Array containing the seed used to derive randomness required for encryption
 * @param[in] pk Array containing the public key
 * @param[inout] state_seedexpander internal state of the seedexpander
 * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
 */

void hqc_pke_encrypt(vector_byte_type u[VEC_N_BYTESIZE],
                     vector_byte_type v[VEC_N1N2_BYTESIZE],
                     vector_byte_type m[VEC_K_BYTESIZE],
                     ap_uint64 theta[KEM_SEED_64BIT_SIZE],
                     ap_uint64 pk[KEY_MAX_64BIT_SIZE],
                     ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                     // Global arrays used to store temporary data
                     vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                     vector_byte_type store_B[VEC_N_BYTESIZE],
                     vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                     vector_index_type random_vector_A[PARAM_OMEGA_R],
                     vector_index_type random_vector_B[PARAM_OMEGA_R],
                     ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {
    // Iterator
	unsigned int i;

    // Retrieve h from public key.
    // Use an intermediate array store_seeds so that all calls to seedexpander_init have an array of
    // size KEM_SEED_64BIT_SIZE as target. This will allow to factorize the calls in hardware to reduce the footprint.
    vect_copyresize_64(store_seeds, KEM_SEED_64BIT_SIZE, pk, KEM_SEED_64BIT_SIZE);
    seedexpander_init(store_seeds, KEM_SEED_64BIT_SIZE, state_seedexpander);
    vect_set_random_mult_type(h_s, state_seedexpander);

    // Initialise the seed expander with theta to generate ciphertext randomness. Use store_seeds again.
    vect_copyresize_64(store_seeds, KEM_SEED_64BIT_SIZE, theta, KEM_SEED_64BIT_SIZE);
    seedexpander_init(store_seeds, KEM_SEED_64BIT_SIZE, state_seedexpander);
	// Generate r1, r2
    vect_set_random_fixed_weight_by_coordinates(random_vector_A, PARAM_OMEGA_R, state_seedexpander);
	vect_set_random_fixed_weight(store_B, PARAM_OMEGA_R, random_vector_A); // store_B <- r1
 	vect_set_random_fixed_weight_by_coordinates(random_vector_A, PARAM_OMEGA_R, state_seedexpander); // random_vector_A <- r2

#ifdef VERBOSE
	printf("\n\nr1: "); vect_print((uint8_t *)store_B, VEC_N_BYTESIZE);
#endif
	// Compute u = store_B + random_vector_A*h_s = r1 + r2*h_s
	// Use store_A as temporary storage for the same reasons as store_seeds was used before.
	// If u is directly used, it leads to multiple instances of vect_mul_add in hardware.
	vect_mul_add(store_A, random_vector_A, h_s, PARAM_OMEGA_R, store_B);
	// Copy the temporary storage to the interface
    vect_copyresize(u, VEC_N_BYTESIZE, store_A, VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE);

    // Generate e
    vect_set_random_fixed_weight_by_coordinates(random_vector_B, PARAM_OMEGA_E, state_seedexpander);
    vect_set_random_fixed_weight(store_B, PARAM_OMEGA_E, random_vector_B); // store_B <- e


	// Compute v = m.G by encoding the message
	code_encode(v, m); // v <- m.G

	// retrieve s from public key
	hqc_public_key_from_string(h_s, pk); // h_s <- s

	// Compute v = v + h_s*random_vector_A + store_B = m.G + s*r2 + e
	vect_mul_add(store_A, random_vector_A, h_s, PARAM_OMEGA_R, store_B);
	vect_add(v, v, store_A, VEC_N1N2_BYTESIZE);


#ifdef VERBOSE
	printf("\n\nh_s: "); vect_print((uint8_t *)h_s, VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER*VECTOR_WORD_BYTESIZE);
	printf("\n\nr2: "); vect_print_sparse((uint16_t *)random_vector_A, PARAM_OMEGA_R);
	printf("\n\ne: "); vect_print((uint8_t *)store_B, VEC_N_BYTESIZE);

	printf("\n\nu: "); vect_print((uint8_t *)u, VEC_N_BYTESIZE);
	printf("\n\nv: "); vect_print((uint8_t *)v, VEC_N1N2_BYTESIZE);
#endif
}


/**
 * @brief Decryption of the HQC_PKE IND_CPA scheme
 *
 * @param[out] m Vector representing the decrypted message
 * @param[in] u Vector u (first part of the ciphertext)
 * @param[in] v Vector v (second part of the ciphertext)
 * @param[in] sk Array containing the secret key
 * @param[inout] state_seedexpander internal state of the seedexpander
 * @param[inout] Multiple vars, arrays for internal computation given in the signature for resource sharing
 */
void hqc_pke_decrypt(vector_byte_type m[VEC_K_BYTESIZE],
                     vector_byte_type u[VEC_N_BYTESIZE],
                     vector_byte_type v[VEC_N1N2_BYTESIZE],
                     ap_uint64 sk[KEY_MAX_64BIT_SIZE],
                     ap_uint64 state_seedexpander[PRNG_STATE_64BIT_SIZE],
                     // Global arrays used to store temporary data
                     vector_byte_type store_A[VEC_N_MULTIPLIERWORDSIZE * MULTIPLIER_BYTESIZE],
                     vector_byte_type store_B[VEC_N_BYTESIZE],
                     vector_word_type h_s[VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER + 1],
                     vector_index_type random_vector_A[PARAM_OMEGA_R],
                     ap_uint64 store_seeds[KEM_SEED_64BIT_SIZE]) {

	// Retrieve y from secret key
	// Use an intermediate array store_seeds so that all calls to seedexpander_init have an array of
    // size KEM_SEED_64BIT_SIZE as target. This will allow to factorize the calls in hardware to reduce the footprint.
	vect_copyresize_64(store_seeds, KEM_SEED_64BIT_SIZE, sk, KEM_SEED_64BIT_SIZE);
	hqc_secret_key_from_string(random_vector_A, store_seeds, state_seedexpander); // random_vector_A <- y


	// Compute v - u.y
	// Again use store_A and store_B as intermediate variables to ensure vect_mul_add is instantiated only once.
    vect_copyresize_mult(h_s, u); // h_s <- u
	vect_copyresize(store_B, VEC_N_BYTESIZE, v, VEC_N1N2_BYTESIZE); // store_B <- v
	vect_mul_add(store_A, random_vector_A, h_s, PARAM_OMEGA, store_B); //store_A <- v - u.y

	// Compute m by decoding v - u.y
	code_decode(m, store_A);


#ifdef VERBOSE
	printf("\n\nu: "); vect_print((uint8_t *)u, VEC_N_BYTESIZE);
	printf("\n\nv: "); vect_print((uint8_t *)v, VEC_N1N2_BYTESIZE);
	printf("\n\ny: "); vect_print_sparse((uint16_t *)random_vector_A, PARAM_OMEGA);
	printf("\n\nv - u.y: "); vect_print((uint8_t *)store_A, VEC_N_BYTESIZE);
#endif

}
