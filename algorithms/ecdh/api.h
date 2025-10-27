#ifndef ECDH_TLS_KEM_H
#define ECDH_TLS_KEM_H

#ifndef ECDH_SECURITY_LEVEL
  #define ECDH_SECURITY_LEVEL 256
#endif

#if ECDH_SECURITY_LEVEL == 256
  #define CRYPTO_ALGNAME "ECDH-256"
  #define CURVE_NAME NID_X9_62_prime256v1
  #define CRYPTO_SECRETKEYBYTES 32
  #define CRYPTO_PUBLICKEYBYTES 65
  #define CRYPTO_CIPHERTEXTBYTES 65
  #define CRYPTO_BYTES 32
#elif ECDH_SECURITY_LEVEL == 384
  #define CRYPTO_ALGNAME "ECDH-384"
  #define CURVE_NAME NID_secp384r1
  #define CRYPTO_SECRETKEYBYTES 48
  #define CRYPTO_PUBLICKEYBYTES 97
  #define CRYPTO_CIPHERTEXTBYTES 97
  #define CRYPTO_BYTES 48
#elif ECDH_SECURITY_LEVEL == 521
  #define CRYPTO_ALGNAME "ECDH-521"
  #define CURVE_NAME NID_secp521r1
  #define CRYPTO_SECRETKEYBYTES 66
  #define CRYPTO_PUBLICKEYBYTES 133
  #define CRYPTO_CIPHERTEXTBYTES 133
  #define CRYPTO_BYTES 64
#else
  #error "Unsupported ECDH_SECURITY_LEVEL (use 256, 384, or 521)"
#endif

int crypto_kem_keypair(unsigned char* pk, unsigned char* sk);
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk);
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk);

#endif