/**
 * @file KeccakF1600_StatePermute.cpp
 * @brief Implementation of KeccakF1600_StatePermute.h
 */

#include "KeccakF1600_StatePermute.h"



#define NROUNDS 24
#define ROL(a, offset) (((a) << (offset)) ^ ((a) >> (64 - (offset))))

/* Keccak round constants */
const ap_uint64 KeccakF_RoundConstants[NROUNDS] = {
	0x0000000000000001ULL, 0x0000000000008082ULL,
	0x800000000000808aULL, 0x8000000080008000ULL,
	0x000000000000808bULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL,
	0x000000000000008aULL, 0x0000000000000088ULL,
	0x0000000080008009ULL, 0x000000008000000aULL,
	0x000000008000808bULL, 0x800000000000008bULL,
	0x8000000000008089ULL, 0x8000000000008003ULL,
	0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800aULL, 0x800000008000000aULL,
	0x8000000080008081ULL, 0x8000000000008080ULL,
	0x0000000080000001ULL, 0x8000000080008008ULL
};


/**
 * @brief Keccak function that permutes the internal state
 *
 * @param[inout] state Keccak state
 */
#ifdef PERF
void KeccakF1600_StatePermute(ap_uint64 state[KECCAK_STATE_64BIT_SIZE]) {

    ap_uint64 E[KECCAK_STATE_64BIT_SIZE];
    ap_uint6 round, i;

    ap_uint64 A[KECCAK_STATE_64BIT_SIZE];
	read_loop : for(i = 0; i < KECCAK_STATE_64BIT_SIZE; ++i){
		A[i] = state[i];
	}


    loop_round: for (round = 0; round < NROUNDS; round += 1) {
    PermuteCompact(A, E, round);
		swap_loop: for (i = 0; i < KECCAK_STATE_64BIT_SIZE; ++i) {
			A[i] = E[i];
		}
    }

    write_loop : for(i = 0; i < KECCAK_STATE_64BIT_SIZE; ++i){
		state[i] = A[i];
	}
}
#else
void KeccakF1600_StatePermute(ap_uint64 state[KECCAK_STATE_64BIT_SIZE]) {
	
	ap_uint64 E[KECCAK_STATE_64BIT_SIZE];
	ap_uint6 round, i;

	loop_round: for (round = 0; round < NROUNDS; round += 1) {
		PermuteCompact(state, E, round);
		swap_loop: for (i = 0; i < KECCAK_STATE_64BIT_SIZE; ++i) {
			state[i] = E[i];
		}
	}
}
#endif


/**
 * @brief Internal core for KeccakF1600_StatePermute that does the actual permutation rounds
 *
 * @param[in] A Keccak input state
 * @param[out] E Keccak output state
 * @param[in] round Integer indicating the current permutation round
 */
void PermuteCompact(ap_uint64 A[KECCAK_STATE_64BIT_SIZE], ap_uint64 E[KECCAK_STATE_64BIT_SIZE], unsigned int round) {
	unsigned int shift_table[KECCAK_STATE_64BIT_SIZE] = { 0,44,43,21,14,28,20,3,45,61,1,6,25,8,18,27,36,10,15,56,62,55,39,41,2 };
	unsigned int indice[5] = {0, 3, 1, 4, 2};
	unsigned int indice_D1[5] = {4, 0, 1, 2, 3};
	unsigned int indice_D2[5] = {1, 2, 3, 4, 0};
	unsigned int indice_E1[5] = {1, 2, 3, 4, 0};
	unsigned int indice_E2[5] = {2, 3, 4, 0, 1};
	
#ifdef COMPACT
	ap_uint64 BCD[15];
#else
	ap_uint64 BC[5];
	ap_uint64 BCl[5];
	ap_uint64 D[5];
#endif


	// iterators
	ap_uint3 BCl_it, BCl_it_2, D_it;
	ap_uint3 m, rolling, BC_it, E_it;

	
	BCl_loop : for (BCl_it = 0; BCl_it < 5; ++BCl_it) {
#ifdef COMPACT
		BCD[BCl_it] = A[0*5+BCl_it];
#else
		BCl[BCl_it] = A[0*5+BCl_it];
#endif
		
		
		BCl_loop_2 : for (BCl_it_2 = 1; BCl_it_2 < 5; ++BCl_it_2) {
#ifdef COMPACT
		BCD[BCl_it] ^= A[BCl_it_2*5+BCl_it];
#else
		BCl[BCl_it] ^= A[BCl_it_2*5+BCl_it];
#endif
		}
	}

	D_loop : for (D_it = 0; D_it < 5; ++D_it) {
#ifdef COMPACT
		BCD[D_it+5] = BCD[indice_D1[D_it]] ^ ROL(BCD[indice_D2[D_it]], 1);
#else
		D[D_it] = BCl[indice_D1[D_it]] ^ ROL(BCl[indice_D2[D_it]], 1);
#endif
	}
	
	main_loop : for (m = 0; m < 5; ++m) {
		rolling = indice[m];
		BC_loop : for (BC_it = 0; BC_it < 5; ++BC_it) {
#ifdef HLS_DATATYPES
	#ifdef COMPACT
			BCD[BC_it+10] = (A[BC_it*5+rolling] ^ BCD[rolling+5]).lrotate(shift_table[m*5+BC_it]);
	#else
			BC[BC_it] = (A[BC_it*5+rolling] ^ D[rolling]).lrotate(shift_table[m*5+BC_it]);
	#endif
#else
	#ifdef COMPACT
			BCD[BC_it+10] = ROL(A[BC_it*5+rolling] ^ BCD[rolling+5], shift_table[m*5+BC_it]);
	#else
			BC[BC_it] = ROL(A[BC_it*5+rolling] ^ D[rolling], shift_table[m*5+BC_it]);
	#endif
#endif
	
			if (++rolling == 5){rolling = 0;}
		}
		E_loop : for (E_it = 0; E_it < 5; ++E_it) {
#ifdef COMPACT
			E[m*5+E_it] = BCD[E_it+10] ^ ((~BCD[indice_E1[E_it]+10]) & BCD[indice_E2[E_it]+10]);
#else
			E[m*5+E_it] = BC[E_it] ^ ((~BC[indice_E1[E_it]]) & BC[indice_E2[E_it]]);
#endif
		}
	}
	
	E[0*5+0] ^= KeccakF_RoundConstants[round];
}

