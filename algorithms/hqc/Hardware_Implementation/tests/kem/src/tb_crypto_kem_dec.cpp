/**
 * @file tb_crypto_kem_dec.cpp
 * @brief Calls the encapsulation and decapsulation functions and checks the shared secret is properly recovered.
 */

#include <stdio.h>
#include "kem.h"
#include "shake_prng.h"

/**
 * @brief Calls the encapsulation and decapsulation functions and checks the shared secret is properly recovered.
 *
 * Initialises the PRNG with 0x43 as initial entropy byte, generates a keypair, increase the prng seed, calls
 * crypto_kem_enc_internal, increases again the prng seed, calls crypto_kem_dec_internal and checks that the plaintext
 * shared secrets returned by the encapsulation and decapsulation functions are the same.
 *
 * @returns 0 if the outputs are the same and the decapsulation function returns 0, and 1 if one of these two conditions is not satisfied
 */
int main() {

    ap_uint64 pk[PUBLIC_KEY_64BIT_SIZE] = {0};
    ap_uint64 sk[SECRET_KEY_64BIT_SIZE] = {0};
    ap_uint64 ss1[SHARED_SECRET_64BIT_SIZE] = {0};
    ap_uint64 ss2[SHARED_SECRET_64BIT_SIZE] = {0};
    ap_uint8 ct[CRYPTO_CIPHERTEXTBYTESIZE] = {0};
    ap_uint8 entropy[1] = {0x43};
    int return_value;

    // iterator
    unsigned int i;


    crypto_kem_keypair_hls(pk, sk, entropy, 1);
    entropy[0]+=1;
    crypto_kem_enc_hls(ct, ss1, pk, entropy, 1);
    return_value = crypto_kem_dec_hls(ss2, ct, sk);

    for(i=0; i < SHARED_SECRET_64BIT_SIZE; ++i) {
        if (ss2[i] != ss1[i] || return_value) {
    		printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			printf("!!!        crypto_kem_decrypt        !!!\n");
			printf("!!!           TESTS FAILED           !!!\n");
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			printf("ss1[%d] : 0x%lx\n", i, (unsigned long)ss1[i]);
            printf("ss2[%d] : 0x%lx\n", i, (unsigned long)ss2[i]);
			printf("Dec return value: %d\n", return_value);
			return 1;
		}
	}
	printf("\n========================================\n");
	printf("===        crypto_kem_decrypt        ===\n");
	printf("===           Tests passed           ===\n");
	printf("========================================\n");
	return 0;
}
