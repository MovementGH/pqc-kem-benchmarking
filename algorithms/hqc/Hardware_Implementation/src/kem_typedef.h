#ifndef KEM_TYPEDEF_H
#define KEM_TYPEDEF_H

#include "parameters.h"

#ifdef HLS_DATATYPES
#include <ap_int.h>

// uint types
typedef ap_uint<1> ap_uint1;
typedef ap_uint<2> ap_uint2;
typedef ap_uint<8> ap_uint3;
typedef ap_uint<8> ap_uint4;
typedef ap_uint<5> ap_uint5;
typedef ap_uint<8> ap_uint6;
typedef ap_uint<8> ap_uint7;
typedef ap_uint<8> ap_uint8;
typedef ap_uint<9> ap_uint9;
typedef ap_uint<10> ap_uint10;
typedef ap_uint<11> ap_uint11;
typedef ap_uint<16> ap_uint12;
typedef ap_uint<16> ap_uint13;
typedef ap_uint<14> ap_uint14;
typedef ap_uint<16> ap_uint15;
typedef ap_uint<16> ap_uint16;
typedef ap_uint<32> ap_uint32;
typedef ap_uint<64> ap_uint64;

// int types
typedef ap_int<6> ap_int6;
typedef ap_int<7> ap_int7;
typedef ap_int<8> ap_int8;
typedef ap_int<10> ap_int10;
typedef ap_int<32> ap_int32;

// gf
typedef ap_uint<8> gf_word_type;
typedef ap_uint<15> gf_dw_type;

// Keccak
typedef ap_uint<6> shift_type;
typedef ap_uint<5> adr_type;
typedef ap_uint<3> len_type;
typedef ap_uint<8> dom_type;

//vector
typedef ap_uint<8> vector_byte_type;
typedef ap_uint<24> random_data_type;
typedef ap_uint<12> vector_size_type;
typedef ap_uint<7> vector_weight_type;
typedef ap_uint<15> vector_index_type;

#if VECTOR_WORD_BITSIZE == 64
typedef ap_uint<64> vector_word_type;
#elif VECTOR_WORD_BITSIZE == 32
typedef ap_uint<32> vector_word_type;
#elif VECTOR_WORD_BITSIZE == 16
typedef ap_uint<16> vector_word_type;
#else
typedef ap_uint<8> vector_word_type;
#endif

// gf2x
#if MULTIPLIER_BYTESIZE == 1
typedef ap_uint<8> multiplier_word_type;
typedef ap_uint<16> multiplier_dw_type;
#elif MULTIPLIER_BYTESIZE == 2
typedef ap_uint<16> multiplier_word_type;
typedef ap_uint<32> multiplier_dw_type;
#elif MULTIPLIER_BYTESIZE == 4
typedef ap_uint<32> multiplier_word_type;
typedef ap_uint<64> multiplier_dw_type;
#elif MULTIPLIER_BYTESIZE == 16
typedef ap_uint<128> multiplier_word_type;
typedef ap_uint<256> multiplier_dw_type;
#elif MULTIPLIER_BYTESIZE == 32
typedef ap_uint<256> multiplier_word_type;
typedef ap_uint<512> multiplier_dw_type;
#elif MULTIPLIER_BYTESIZE == 64
typedef ap_uint<512> multiplier_word_type;
typedef ap_uint<1024> multiplier_dw_type;
#else
typedef ap_uint<64> multiplier_word_type;
typedef ap_uint<128> multiplier_dw_type;
#endif

#else // HLS_DATATYPES
// #include <stdint.h>
// uint types
typedef uint8_t ap_uint1;
typedef uint8_t ap_uint2;
typedef uint8_t ap_uint3;
typedef uint8_t ap_uint4;
typedef uint8_t ap_uint5;
typedef uint8_t ap_uint6;
typedef uint8_t ap_uint8;
typedef uint8_t ap_uint7;
typedef uint16_t ap_uint9;
typedef uint16_t ap_uint10;
typedef uint16_t ap_uint11;
typedef uint16_t ap_uint12;
typedef uint16_t ap_uint13;
typedef uint16_t ap_uint14;
typedef uint16_t ap_uint15;
typedef uint16_t ap_uint16;
typedef uint32_t ap_uint32;
typedef uint64_t ap_uint64;

// int types
typedef int8_t ap_int6;
typedef int8_t ap_int7;
typedef int8_t ap_int8;
typedef int16_t ap_int10;
typedef int32_t ap_int32;

// gf
typedef uint8_t gf_word_type;
typedef uint16_t gf_dw_type;

// gf2x
#if MULTIPLIER_BYTESIZE == 1
typedef uint8_t multiplier_word_type;
#elif MULTIPLIER_BYTESIZE == 2
typedef uint16_t multiplier_word_type;
#elif MULTIPLIER_BYTESIZE == 4
typedef uint32_t multiplier_word_type;
#else
typedef uint64_t multiplier_word_type;
#endif

// Keccak
typedef uint8_t shift_type;
typedef uint8_t adr_type;
typedef uint8_t len_type;
typedef uint8_t dom_type;

// vector
typedef uint8_t vector_byte_type;
typedef uint32_t random_data_type;
typedef uint16_t vector_size_type;
typedef uint8_t vector_weight_type;
typedef uint16_t vector_index_type;

#if VECTOR_WORD_BITSIZE == 64
typedef uint64_t vector_word_type;
#elif VECTOR_WORD_BITSIZE == 32
typedef uint32_t vector_word_type;
#elif VECTOR_WORD_BITSIZE == 16
typedef uint16_t vector_word_type;
#else
typedef uint8_t vector_word_type;
#endif

#endif //#ifdef HLS_DATATYPES

#endif //KEM_TYPEDEF_H
