/**
 * @file api.cpp
 * @brief NIST KEM API implementation used by the HQC_KEM IND-CCA2 scheme
 */

#include "api.h"
#include "kem.h"

/**
 * @brief Keygen of the HQC_KEM IND_CAA2 scheme, API version
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
void crypto_kem_keypair(uint8_t pk[CRYPTO_PUBLICKEYBYTESIZE], uint8_t sk[CRYPTO_SECRETKEYBYTESIZE], uint8_t *prng_seed, uint8_t seed_bytesize){
    crypto_kem_keypair_hls((ap_uint64 *)pk, (ap_uint64 *)sk, (ap_uint8 *)prng_seed, (ap_uint8)seed_bytesize);
}

/**
 * @brief Encapsulation function of the HQC_KEM IND_CAA2 scheme, API version
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
void crypto_kem_enc(uint8_t ct[CRYPTO_CIPHERTEXTBYTESIZE], uint8_t ss[CRYPTO_SHARED_SECRET_BYTESIZE], uint8_t pk[CRYPTO_PUBLICKEYBYTESIZE], uint8_t *prng_seed, uint8_t seed_bytesize ) {
    crypto_kem_enc_hls((ap_uint8 *) ct, (ap_uint64 *)ss, (ap_uint64 *)pk, (ap_uint8 *)prng_seed, (ap_uint8)seed_bytesize);
}

/**
 * @brief Decapsulation function of the HQC_KEM IND_CAA2 scheme, API version
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
int crypto_kem_dec(uint8_t ss[CRYPTO_SHARED_SECRET_BYTESIZE], uint8_t ct[CRYPTO_CIPHERTEXTBYTESIZE], uint8_t sk[CRYPTO_SECRETKEYBYTESIZE]){
    return crypto_kem_dec_hls((ap_uint64 *)ss, (ap_uint8 *)ct, (ap_uint64 *)sk);
}
