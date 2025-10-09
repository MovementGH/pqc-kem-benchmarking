
#ifndef HQC_PARAMETERS_H
#define HQC_PARAMETERS_H

/**
 * @file parameters.h
 * @brief Parameters of the HQC_KEM IND-CCA2 scheme
 */

#include "api.h"


#define CEIL_DIVIDE(a, b)  (((a)/(b)) + ((a) % (b) == 0 ? 0 : 1)) /*!< Divide a by b and ceil the result*/
#define BITMASK(a, size) ((1ULL << ((a) % (size))) - 1) /*!< Create a mask*/

/*
  #define PARAM_N                             	Define the parameter n of the scheme
  #define PARAM_N1                            	Define the parameter n1 of the scheme (length of Reed-Solomon code)
  #define PARAM_N2                            	Define the parameter n2 of the scheme (length of Duplicated Reed-Muller code)
  #define PARAM_N1N2                          	Define the length in bits of the Concatenated code
  #define PARAM_OMEGA                         	Define the parameter omega of the scheme
  #define PARAM_OMEGA_E                       	Define the parameter omega_e of the scheme
  #define PARAM_OMEGA_R                       	Define the parameter omega_r of the scheme

  #define SECRET_KEY_BYTESIZE                   Define the size of the secret key in bytes
  #define PUBLIC_KEY_BYTESIZE                   Define the size of the public key in bytes
  #define SHARED_SECRET_BYTESIZE                Define the size of the shared secret in bytes
  #define SHARED_SECRET_64BIT_SIZE              Define the size of the shared secret in 64-bit words
  #define CIPHERTEXT_BYTESIZE                   Define the size of the ciphertext in bytes

  #define UTILS_REJECTION_THRESHOLD           	Define the rejection threshold used to generate given weight vectors (see vector_set_random_fixed_weight function)
  #define VEC_N_BYTESIZE                    	Define the size of the array used to store a PARAM_N sized vector in bytes
  #define VEC_K_BYTESIZE                    	Define the size of the array used to store a PARAM_K sized vector in bytes
  #define VEC_N1N2_BYTESIZE                 	Define the size of the array used to store a PARAM_N1N2 sized vector in bytes
  
  #define MULTIPLIER_BITSIZE                    Size of the operator in vect_mul, in bits
  #define MULTIPLIER_BYTESIZE                   Size of the operator in vect_mul, in bytes
  #define VEC_N_MULTIPLIERWORDSIZE              Define the size of the array used to store a PARAM_N sized vector in multiplier words

  #define PARAM_DELTA                         	Define the parameter delta of the scheme (correcting capacity of the Reed-Solomon code)
  #define PARAM_M                             	Define the extension degree for the basic galois field
  #define PARAM_K                             	Define the size of the information bits of the Reed-Solomon code
  #define PARAM_G                             	Define the size of the generator polynomial of Reed-Solomon code
  #define PARAM_FFT                           	The additive FFT takes a 2^PARAM_FFT polynomial as input
  #define RS_POLY_COEFS                       	Coefficients of the generator polynomial of the Reed-Solomon code

  #define RED_MASK                            	A mask fot the higher bits of a vector
  #define SHAKE256_512_BYTESIZE                 Define the size of SHAKE256_512 output in bytes
  #define KEM_SEED_BYTESIZE                     Define the size of the seed in bytes
*/

#define RM_BYTESIZE                             16
#define RM_BITSIZE                              (RM_BYTESIZE*8)
#define NB_OF_RMS                               3
#define PARAM_N1                            	46
#define PARAM_N2                            	(RM_BITSIZE*NB_OF_RMS)



#define PARAM_N					                17669
#define PARAM_N1N2                          	17664
#define UTILS_REJECTION_THRESHOLD           	16767881

#define PARAM_OMEGA                         	66
#define PARAM_OMEGA_E                       	75
#define PARAM_OMEGA_R                       	75

#define SECRET_KEY_BYTESIZE                    	CRYPTO_SECRETKEYBYTESIZE
#define SECRET_KEY_64BIT_SIZE                   CEIL_DIVIDE(SECRET_KEY_BYTESIZE,8)
#define PUBLIC_KEY_BYTESIZE                    	CRYPTO_PUBLICKEYBYTESIZE
#define PUBLIC_KEY_64BIT_SIZE                   CEIL_DIVIDE(CRYPTO_PUBLICKEYBYTESIZE, 8)
#define SHARED_SECRET_BYTESIZE                 	CRYPTO_SHARED_SECRET_BYTESIZE
#define SHARED_SECRET_64BIT_SIZE                CEIL_DIVIDE(CRYPTO_SHARED_SECRET_BYTESIZE,8)
#define CIPHERTEXT_BYTESIZE                    	CRYPTO_CIPHERTEXTBYTESIZE

#define KEY_MAX_64BIT_SIZE                      ((SECRET_KEY_64BIT_SIZE > PUBLIC_KEY_64BIT_SIZE) ? SECRET_KEY_64BIT_SIZE : PUBLIC_KEY_64BIT_SIZE)

#define VEC_N_BYTESIZE                    	    CEIL_DIVIDE(PARAM_N, 8)
#define VEC_K_BYTESIZE                    	    PARAM_K
#define VEC_N1_BYTESIZE                   	    PARAM_N1
#define VEC_N1N2_BYTESIZE                 	    CEIL_DIVIDE(PARAM_N1N2, 8)


#define VEC_N_64BIT_SIZE                       	CEIL_DIVIDE(PARAM_N, 64)
#define VEC_K_64BIT_SIZE                       	CEIL_DIVIDE(PARAM_K, 8)
#define PRNG_MAX_OUTPUT_BYTESIZE                VEC_N_BYTESIZE

#ifdef HLS_DATATYPES
// !!!!!!!!!!!!!!!!  max condition here:  MULTIPLIER_BITSIZE <= 512 !!!!!!!!!!!!!!!!!!!!!!*/
#ifdef COMPACT
#define MULTIPLIER_BITSIZE                               8
#else
#define MULTIPLIER_BITSIZE                               256
#endif
#else
// !!!!!!!!!!!!!!!!  max condition here:  MULTIPLIER_BITSIZE <= 64 !!!!!!!!!!!!!!!!!!!!!!*/
#ifdef COMPACT
  #define MULTIPLIER_BITSIZE                    8
  #else
  #define MULTIPLIER_BITSIZE                    64
  #endif
#endif
#define MULTIPLIER_BYTESIZE                     (MULTIPLIER_BITSIZE / 8)
#define VEC_N_MULTIPLIERWORDSIZE                CEIL_DIVIDE(PARAM_N, MULTIPLIER_BITSIZE)

// !!!!!!!!!!!!!!!!  max condition here:  VECTOR_WORD_BITSIZE <= 32 !!!!!!!!!!!!!!!!!!!!!!*/
#ifdef COMPACT
#define VECTOR_WORD_BITSIZE                     8
#else
#define VECTOR_WORD_BITSIZE                     32
#endif
#define VECTOR_WORD_BYTESIZE                    (VECTOR_WORD_BITSIZE / 8)

#define MULTIPLIER_VECTORWORDSIZE               (MULTIPLIER_BITSIZE / VECTOR_WORD_BITSIZE)
#define VEC_N_VECTORWORDSIZE                    CEIL_DIVIDE(PARAM_N, VECTOR_WORD_BITSIZE)
#define VEC_N_VECTORWORDSIZE_FOR_MULTIPLIER     CEIL_DIVIDE(VEC_N_VECTORWORDSIZE, MULTIPLIER_VECTORWORDSIZE)*MULTIPLIER_VECTORWORDSIZE

#define PARAM_DELTA                         	15
#define PARAM_M                            	    8
#define PARAM_K                             	16
#define PARAM_G                             	31
#define PARAM_FFT                           	5
#define RS_POLY_COEFS                           89,69,153,116,176,117,111,75,73,233,242,233,65,210,21,139,103,173,67,118,105,210,174,110,74,69,228,82,255,181,1

#define RED_MASK                            	BITMASK(PARAM_N, sizeof(vector_byte_type)*8)
#define SHAKE256_512_BYTESIZE                   64
#define SHAKE256_512_64BIT_SIZE                 CEIL_DIVIDE(SHAKE256_512_BYTESIZE,8)
#define KEM_SEED_BYTESIZE                       40
#define KEM_SEED_64BIT_SIZE                     CEIL_DIVIDE(KEM_SEED_BYTESIZE, 8)

#endif
