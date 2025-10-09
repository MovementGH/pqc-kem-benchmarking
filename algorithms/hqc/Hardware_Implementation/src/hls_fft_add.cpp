/**
 * @file hls_fft_add.c
 * @brief Implementation of the additive FFT and its transpose.
 * This implementation is based on the paper from Gao and Mateer: <br>
 * Shuhong Gao and Todd Mateer, Additive Fast Fourier Transforms over Finite Fields,
 * IEEE Transactions on Information Theory 56 (2010), 6265--6272.
 * http://www.math.clemson.edu/~sgao/papers/GM10.pdf <br>
 * and includes improvements proposed by Bernstein, Chou and Schwabe here:
 * https://binary.cr.yp.to/mcbits-20130616.pdf
 *
 * Functions in this files are hardcoded for an input array of 32 elemetns and an output array of 256.
 * No macro is therefore used in the signatures to indicate this fact.
 *
 */

#include "hls_fft_add.h"

/**
 * @brief Subroutine of the fft function that computes the last butterflies over 16 coeffs
 *
 * @param[out] small_w Output
 * @param[in] A_0 Input polynomial
 * @param[in] A_1 Input polynomial
 */
void leaves_butterfly(ap_uint8 small_w[16], ap_uint8 A_0, ap_uint8 A_1) {
	ap_uint5 j;
	ap_uint3 i;
	ap_uint8 tmp[4];

	const ap_uint8 betas[4] = {0x08, 0x54, 0x9d, 0x4e};

	loop_gf_mul: for (i = 0; i < 4; ++i) {
		tmp[i] = gf_mul(betas[i], A_1);
	}

	small_w[0] = A_0;
	
	loop_small_w: for (i = 0; i < 4; ++i) {
		loop_small_w_1: for (j = 0; j < (1 << i); ++j) {
			small_w[(1 << i) + j] = small_w[j] ^ tmp[i];
		}
	}
	
	return;
}


/**
 * @brief Computes the radix conversion of a four coordinate polynomial _A

 * @param[out] _B Radix conversion
 * @param[in] _A Input polynomial
 */
void radix(ap_uint8 _B[4], ap_uint8 _A[4]){ // _B[0..1] = R, _B[2..3] = Q
	_B[0] = _A[0];
	_B[1] = _A[1] ^ _A[2] ^ _A[3];
	_B[2] = _A[2] ^ _A[3];
	_B[3] = _A[3];
}

/**
 * @brief Evaluates f on all fields elements using an additive FFT algorithm
 *
 * f_coeffs is the number of coefficients of f (one less than its degree). <br>
 * The FFT proceeds recursively to evaluate f at all subset sums of a basis B. <br>
 * This implementation is based on the paper from Gao and Mateer: <br>
 * Shuhong Gao and Todd Mateer, Additive Fast Fourier Transforms over Finite Fields,
 * IEEE Transactions on Information Theory 56 (2010), 6265--6272.
 * http://www.math.clemson.edu/~sgao/papers/GM10.pdf <br>
 * and includes improvements proposed by Bernstein, Chou and Schwabe here:
 * https://binary.cr.yp.to/mcbits-20130616.pdf <br>
 *
 * @param[out] small_w Array with the output values
 * @param[in] f Array of 2^PARAM_FFT == 32 elements
 */
void add_fft(ap_uint8 small_w[256], ap_uint8 f[32]) {

    // This fft implementation is hardcoded for a 256 elements evaluation and a polynomial of degree below 32
    // Use two assertions so that any changes on these parameters raises an alert
    assert(PARAM_M == 8);
    assert(PARAM_FFT == 5);

	ap_uint8 fr[32];

	/// Constants ///

	ap_uint8 perm_r[128] =  {0, 2, 4, 6, 8, 10, 12, 14,
                             16, 18, 20, 22, 24, 26, 28, 30,
                             1, 3, 5, 7, 9, 11, 13, 15,
                             17, 19, 21, 23, 25, 27, 29, 31,

                             0, 2, 4, 6, 8, 10, 12, 14,
                             1, 3, 5, 7, 9, 11, 13, 15,
                             16, 18, 20, 22, 24, 26, 28, 30,
                             17, 19, 21, 23, 25, 27, 29, 31,

                             0, 2, 4, 6, 1, 3, 5, 7,
                             8, 10, 12, 14, 9, 11, 13, 15,
                             16, 18, 20, 22, 17, 19, 21, 23,
                             24, 26, 28, 30, 25, 27, 29, 31,

                             0, 2, 1, 3, 4, 6, 5, 7,
                             8, 10, 9, 11, 12, 14, 13, 15,
                             16, 18, 17, 19, 20, 22, 21, 23,
                             24, 26, 25, 27, 28, 30, 29, 31};

	ap_uint8 bm_pow_R[28] = {0x01, 0x06, 0x14, 0x78,
                             0x0d, 0x2e, 0xe4, 0x62,
                             0x51, 0xfb, 0x20, 0xc0,
                             0xba, 0xbb, 0xbd, 0xa9,

                             0x01, 0x12, 0x19, 0xbf,
                             0x5c, 0x11, 0x2f, 0x94,

                             0x01, 0x1f, 0x48, 0x6b};

	ap_uint8 gammas_sums_R[240] = {0x00, 0x2d, 0x21, 0x0c, 0xae, 0x83, 0x8f, 0xa2,
                                   0x0b, 0x26, 0x2a, 0x07, 0xa5, 0x88, 0x84, 0xa9,

                                   0x00, 0x0c, 0xb7, 0xbb, 0x26, 0x2a, 0x91, 0x9d,
                                   0x61, 0x6d, 0xd6, 0xda, 0x47, 0x4b, 0xf0, 0xfc,
                                   0x16, 0x1a, 0xa1, 0xad, 0x30, 0x3c, 0x87, 0x8b,
                                   0x77, 0x7b, 0xc0, 0xcc, 0x51, 0x5d, 0xe6, 0xea,

                                   0x00, 0xb6, 0xb3, 0x05, 0xed, 0x5b, 0x5e, 0xe8,
                                   0x78, 0xce, 0xcb, 0x7d, 0x95, 0x23, 0x26, 0x90,
                                   0x1c, 0xaa, 0xaf, 0x19, 0xf1, 0x47, 0x42, 0xf4,
                                   0x64, 0xd2, 0xd7, 0x61, 0x89, 0x3f, 0x3a, 0x8c,
                                   0x06, 0xb0, 0xb5, 0x03, 0xeb, 0x5d, 0x58, 0xee,
                                   0x7e, 0xc8, 0xcd, 0x7b, 0x93, 0x25, 0x20, 0x96,
                                   0x1a, 0xac, 0xa9, 0x1f, 0xf7, 0x41, 0x44, 0xf2,
                                   0x62, 0xd4, 0xd1, 0x67, 0x8f, 0x39, 0x3c, 0x8a,

                                   0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
                                   0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
                                   0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
                                   0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
                                   0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
                                   0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
                                   0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
                                   0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
                                   0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
                                   0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
                                   0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
                                   0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
                                   0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
                                   0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
                                   0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
                                   0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe};

	ap_uint8 _A[32], big_W[256], _B[4], _C[4], w16[16];
	ap_uint8 A_local;
	ap_uint3 k, l;
	ap_uint6 i, n;
	ap_uint4 m;
	ap_uint9 j;
	ap_uint4 ind_1;
	ap_uint2 ind_2;
	ap_uint3 round = 0;
	ap_uint8 adr = 0xc0;

	// Store input data
	memset_A : for(i=0;i<32;++i) _A[i] = f[i];
	
	/// Round 1 to 4 ///
	part_1_round : for (round = 0; round < 4; round++) {
		if (round != 0) {
			memset_a_fr_1_1: for(i = 0; i < 32;++i) fr[i] = _A[perm_r[i + (32 * (round-1))]];
			memset_a_fr_1_2: for(i = 0; i < 32;++i) _A[i] = fr[i];
		}

		ind_1 = 5 - round;
		ind_2 = 3 - round;
		part_1_round_1: for (k = 0; k < (4 - round) ; ++k){
			assert(ind_2 <= 3);
			for(i = 0; i < (1 << ind_2); ++i) {
				round_1_4 : for(j = 0; j < (1 << k);++j) {
					round_1_4_1: for (l = 0; l < 4; ++l) {
						if (k == 0 && round != 0)
							_B[l] = gf_mul(_A[(j << ind_1) + (l << ind_2) + i], bm_pow_R[((adr >> round) & 0x1f) + (l << ind_2) +i]);
						else
							_B[l] =_A[(j << ind_1) + (l << ind_2) + i];
					}
					
					radix(_C,_B);
					round_1_4_2: for (l = 0; l < 4; ++l) {
						_A[(j << ind_1) + (l << ind_2) + i] = _C[l];
					}
				}
			}
			ind_1--;
			ind_2--;
		}
	}


	
	/// Butterfly on leaves ///
	butterfly_loops : for(i = 0; i < 8;++i) {
		leaves_butterfly(w16, _A[4*i+0], _A[4*i+1]);
		Butterfly_1 : for(n =  0; n < 16; ++n) big_W[32*i+n] =  w16[n];
		Butterfly_2 : for(n = 16; n < 32; ++n) big_W[32*i+n] =  0;
	}

	/// Round 1 to 4 ///
	ind_2 = 3;
	ind_1 = 5;
	part_2_round : for (round = 0; round < 4; round++) {
		assert(ind_2 <= 4);
		for(m = 0; m < (1 << ind_2);++m) {
			if (round == 0)
				A_local = _A[ 4 * m + 2];
			else
				A_local = big_W[(m << ind_1) + (16 << round)];
			big_W[(m << ind_1) + (16 << round)] = big_W[(m << ind_1)] ^ A_local;
			Butterfly_1_1: for(j = 1 ; j < (16 << round) ; ++j) {
				if (round != 0) 
					A_local = big_W[(m << ind_1) + (16 << round) + j];
				big_W[(m << ind_1)                 + j] = big_W[(m << ind_1) + j] ^ gf_mul(gammas_sums_R[j + (16 * round)], A_local);
				big_W[(m << ind_1) + (16 << round) + j] = big_W[(m << ind_1) + j] ^ A_local;
				
			}
		}
		ind_2--;
		ind_1++;
	}

	
	
	small_w_loop : for(j = 0; j < 256;++j) small_w[j] = big_W[j];
	return;
}

/**
 * @brief Retrieves the error polynomial error from the evaluations w of the ELP (Error Locator Polynomial) on all field elements.
 *
 * @param[out] error Array with the output errors
 * @param[in] small_w Array of with the input evaluations
 */
void add_fft_retrieve_error_poly(ap_uint8 error[256], const ap_uint8 small_w[256]) {
	ap_uint7 i;
	ap_uint2 j;

	ap_uint8 index[256] = {0x00, 0xf8, 0xf9, 0xe0, 0xfa, 0xc8, 0xe1, 0x34,
                           0xfb, 0x1c, 0xc9, 0x93, 0xe2, 0x0d, 0x35, 0xb0,
                           0xfc, 0x98, 0x1d, 0x3b, 0xca, 0x6f, 0x94, 0xf4,
                           0xe3, 0xee, 0x0e, 0x04, 0x36, 0x7b, 0xb1, 0x8b,
                           0xfd, 0x73, 0x99, 0x48, 0x1e, 0x6a, 0x3c, 0x63,
                           0xcb, 0xd9, 0x70, 0xd6, 0x95, 0xeb, 0xf5, 0x19,
                           0xe4, 0xce, 0xef, 0x80, 0x0f, 0x23, 0x05, 0x85,
                           0x37, 0xdc, 0x7c, 0x44, 0xb2, 0xb8, 0x8c, 0x57,
                           0xfe, 0x3f, 0x74, 0xbc, 0x9a, 0x2e, 0x49, 0xa0,
                           0x1f, 0x66, 0x6b, 0xc4, 0x3d, 0x2c, 0x64, 0x2a,
                           0xcc, 0x21, 0xda, 0xb6, 0x71, 0x68, 0xd7, 0xe9,
                           0x96, 0x6d, 0xec, 0x79, 0xf6, 0xc6, 0x1a, 0x0b,
                           0xe5, 0x9c, 0xcf, 0x5b, 0xf0, 0x30, 0x81, 0x5f,
                           0x10, 0x4b, 0x24, 0xaa, 0x06, 0xa2, 0x86, 0x52,
                           0x38, 0x01, 0xdd, 0x90, 0x7d, 0x41, 0x45, 0xd3,
                           0xb3, 0x76, 0xb9, 0xc1, 0x8d, 0xbe, 0x58, 0xa7,
                           0x00, 0x8f, 0x40, 0xd2, 0x75, 0xc0, 0xbd, 0xa6,
                           0x9b, 0x5a, 0x2f, 0x5e, 0x4a, 0xa9, 0xa1, 0x51,
                           0x20, 0xb5, 0x67, 0xe8, 0x6c, 0x78, 0xc5, 0x0a,
                           0x3e, 0xbb, 0x2d, 0x9f, 0x65, 0xc3, 0x2b, 0x29,
                           0xcd, 0x7f, 0x22, 0x84, 0xdb, 0x43, 0xb7, 0x56,
                           0x72, 0x47, 0x69, 0x62, 0xd8, 0xd5, 0xea, 0x18,
                           0x97, 0x3a, 0x6e, 0xf3, 0xed, 0x03, 0x7a, 0x8a,
                           0xf7, 0xdf, 0xc7, 0x33, 0x1b, 0x92, 0x0c, 0xaf,
                           0xe6, 0x08, 0x9d, 0x27, 0xd0, 0xa4, 0x5c, 0x4f,
                           0xf1, 0x88, 0x31, 0xad, 0x82, 0x54, 0x60, 0x16,
                           0x11, 0x12, 0x4c, 0x13, 0x25, 0x4d, 0xab, 0x14,
                           0x07, 0x26, 0xa3, 0x4e, 0x87, 0xac, 0x53, 0x15,
                           0x39, 0xf2, 0x02, 0x89, 0xde, 0x32, 0x91, 0xae,
                           0x7e, 0x83, 0x42, 0x55, 0x46, 0x61, 0xd4, 0x17,
                           0xb4, 0xe7, 0x77, 0x09, 0xba, 0x9e, 0xc2, 0x28,
                           0x8e, 0xd1, 0xbf, 0xa5, 0x59, 0x5d, 0xa8, 0x50};

	retreive_error: for (i = 0 ; i < 128 ; ++i) {
		retreive_error_1: for (j = 0; j < 2; ++j) {
			error[index[i + (j * 128)]] ^= (ap_uint8)(((ap_uint9)1) ^ ((ap_uint9)-small_w[i + (j * 128)] >> FFT_SHIFT));
		}
	}
}


/**
 * @brief Computes the error polynomial error from the error locator polynomial sigma
 *
 * See function add_fft for more details.
 *
 * @param[out] error Array of 2^PARAM_M == 256 elements receiving the error polynomial
 * @param[in] sigma Array of 2^PARAM_FFT == 32 elements storing the error locator polynomial
 */
void compute_roots(ap_uint8 error[256], ap_uint8 sigma[32]) {
	ap_uint8 small_w[256];
	add_fft(small_w, sigma);
	add_fft_retrieve_error_poly(error, small_w);

}
