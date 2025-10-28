#include "api.h"
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdlib.h>

/* Generate keypair */
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
    /* Generate keypair */
    EC_GROUP *group = EC_GROUP_new_by_curve_name(CURVE_NAME);
    EC_KEY *keypair = EC_KEY_new();
    EC_KEY_set_group(keypair, group);
    EC_KEY_generate_key(keypair);

    /* Export public key to buffer */
    EC_POINT *pub = EC_KEY_get0_public_key(keypair);
    EC_POINT_point2oct(group, pub, POINT_CONVERSION_UNCOMPRESSED, pk, CRYPTO_PUBLICKEYBYTES, NULL);

    /* Export private key to buffer */
    BIGNUM *priv = EC_KEY_get0_private_key(keypair);
    BN_bn2binpad(priv, sk, CRYPTO_SECRETKEYBYTES);

    EC_KEY_free(keypair);
    return 0;
}

/* Encapsulate */
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    /* Generate ephemeral keypair */
    EC_GROUP *group = EC_GROUP_new_by_curve_name(CURVE_NAME);
    EC_KEY *eph_keypair = EC_KEY_new();
    EC_KEY_set_group(eph_keypair, group);
    EC_KEY_generate_key(eph_keypair);

    /* Import peer public key */
    EC_POINT *peer_pub = EC_POINT_new(group);
    EC_POINT_oct2point(group, peer_pub, pk, CRYPTO_PUBLICKEYBYTES, NULL);

    /* Compute shared secret from the ephemeral private key and the peer's public key */
    ECDH_compute_key(ss, CRYPTO_BYTES, peer_pub, eph_keypair, NULL);

    /* Export ephemeral public key to buffer to be sent to peer */
    EC_POINT *eph_pub = EC_KEY_get0_public_key(eph_keypair);
    EC_POINT_point2oct(group, eph_pub, POINT_CONVERSION_UNCOMPRESSED, ct, CRYPTO_CIPHERTEXTBYTES, NULL);

    EC_POINT_free(peer_pub);
    EC_KEY_free(eph_keypair);
    EC_GROUP_free(group);
    return 0;
}

/* Decapsulate */
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    /* Import private key from buffer */
    EC_GROUP *group = EC_GROUP_new_by_curve_name(CURVE_NAME);
    EC_KEY *keypair = EC_KEY_new();
    EC_KEY_set_group(keypair, group);

    BIGNUM *priv = BN_bin2bn(sk, CRYPTO_SECRETKEYBYTES, NULL);
    EC_KEY_set_private_key(keypair, priv);

    /* Import peer public key */
    EC_POINT *peer_pub = EC_POINT_new(group);
    EC_POINT_oct2point(group, peer_pub, ct, CRYPTO_CIPHERTEXTBYTES, NULL);

    /* Compute shared secret from private key and peer's public key */
    ECDH_compute_key(ss, CRYPTO_BYTES, peer_pub, keypair, NULL);

    EC_POINT_free(peer_pub);
    EC_KEY_free(keypair);
    EC_GROUP_free(group);
    BN_free(priv);
    return 0;
}