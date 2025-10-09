/**
 * @file tb_crypto_kem_all_functions.cpp
 * @brief Calls a wrapper to the encapsulation and decapsulation functions and checks the shared secret is properly recovered.
 *
 * Does the same as tb_crypto_kem_dec.cpp but using a wrapper. Useful to indicate HLS that all the sub-functions need
 * to be translated to hardware (in tb_crypto_kem_dec.cpp HLS only translates the crypto_kem_dec_internal function).
 */

#include "kem.h"
#include <stdio.h>


/**
 * @brief Calls the encapsulation and decapsulation functions and checks the shared secret is properly recovered.
 *
 * Initialises the PRNG with 0x43 as initial entropy byte, calls crypto_kem_all_functions_all and checks that the
 * two returned plaintext shared secrets are the same.
 *
 * @returns 0 if the outputs are the same and crypto_kem_all_functions returns 0, or 1 if one of these two conditions is not satisfied
 */
int main() {
    ap_uint64 ss1[SHARED_SECRET_64BIT_SIZE] = {0 };
    ap_uint64 ss2[SHARED_SECRET_64BIT_SIZE] = {0 };
    ap_uint8 entropy[1] = {0x43};
    int return_value;

    return_value = crypto_kem_all_functions_hls(ss1, ss2, entropy, 1);
    for (int i = 0; i < SHARED_SECRET_64BIT_SIZE; ++i) {
        if (ss2[i] != ss1[i] || return_value) {
            printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("!!!     crypto_kem_all_functions_hls      !!!\n");
            printf("!!!           TESTS FAILED            !!!\n");
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            printf("ss1[%d] : 0x%lx\n", i, (unsigned long)ss1[i]);
            printf("ss2[%d] : 0x%lx\n", i, (unsigned long)ss2[i]);
            printf("Dec return value: %d\n", return_value);
            return 1;
        }
    }

    
    printf("\n========================================\n");
    printf("===     crypto_kem_all_functions_hls    \n");
	printf("===           Tests passed           ===\n");
	printf("========================================\n");
    return 0;


}
