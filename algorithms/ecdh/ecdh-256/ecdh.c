#include "api.h"
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdlib.h>

/* Generate keypair */
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
    if (RAND_bytes(sk, CRYPTO_SECRETKEYBYTES) <= 0)
        return -1;

    EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_EC, NULL, sk, CRYPTO_SECRETKEYBYTES);
    if (!pkey)
        return -1;

    size_t len = CRYPTO_PUBLICKEYBYTES;
    int ok = EVP_PKEY_get_raw_public_key (pkey, pk, &len) > 0;

    EVP_PKEY_free (pkey);
    return ok ? 0 : -1;
}

/* Encapsulate */
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    unsigned char eph_sk[CRYPTO_SECRETKEYBYTES];
    if (RAND_bytes(eph_sk, sizeof(eph_sk)) <= 0)
        return -1;

    EVP_PKEY *eph = EVP_PKEY_new_raw_private_key(EVP_PKEY_EC, NULL, eph_sk, sizeof(eph_sk));
    if (!eph)
        return -1;

    size_t ct_len = CRYPTO_CIPHERTEXTBYTES;
    if (EVP_PKEY_get_raw_public_key(eph, ct, &ct_len) <= 0)
        goto fail;

    EVP_PKEY *peer = EVP_PKEY_new_raw_public_key(EVP_PKEY_EC, NULL, pk, CRYPTO_PUBLICKEYBYTES);
    if (!peer)
        goto fail;

    size_t ss_len = CRYPTO_BYTES;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new (eph, NULL);
    if (!ctx)
        goto fail2;

    if (EVP_PKEY_derive_init(ctx) <= 0 ||
        EVP_PKEY_derive_set_peer(ctx, peer) <= 0 ||
        EVP_PKEY_derive(ctx, ss, &ss_len) <= 0)
        goto fail3;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peer);
    EVP_PKEY_free(eph);
    return 0;

fail3: 
    EVP_PKEY_CTX_free(ctx);
fail2:
    EVP_PKEY_free(peer);
fail:
    EVP_PKEY_free(eph);
    return -1;
}

/* Decapsulate */
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    EVP_PKEY *our = EVP_PKEY_new_raw_private_key(EVP_PKEY_EC, NULL, sk, CRYPTO_SECRETKEYBYTES);
    if (!our)
        return -1;

    EVP_PKEY *peer = EVP_PKEY_new_raw_public_key(EVP_PKEY_EC, NULL, ct, CRYPTO_CIPHERTEXTBYTES);
    if (!peer) {
        EVP_PKEY_free(our);
        return -1;
    }

    size_t ss_len = CRYPTO_BYTES;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new (our, NULL);
    if (!ctx ||
        EVP_PKEY_derive_init(ctx) <= 0 ||
        EVP_PKEY_derive_set_peer(ctx, peer) <= 0 ||
        EVP_PKEY_derive(ctx, ss, &ss_len) <= 0)
        goto fail;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peer);
    EVP_PKEY_free(our);
    return 0;

fail:
    if (ctx) EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(peer);
    EVP_PKEY_free(our);
    return -1;
}