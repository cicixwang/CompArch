#include <stdlib.h>

#include <immintrin.h>

#include "dot_product.h"
/* reference implementation in C */
unsigned int dot_product_C(long size, unsigned short * a, unsigned short *b) {
    unsigned int sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// add prototypes here!
extern unsigned int dot_product_gcc7_O3(long size, unsigned short * a, unsigned short *b);

unsigned int dot_product_AVX(long size, unsigned short * a, unsigned short *b) {
    	__m256i partial_sums = _mm256_setzero_si256();
	for (int i=0; i<size; i+=8) {
		__m128i a_part = _mm_loadu_si128((__m128i*) &a[i]);
		__m128i b_part = _mm_loadu_si128((__m128i*) &b[i]);
		__m256i a_part_converted = _mm256_cvtepu16_epi32(a_part);
		__m256i b_part_converted = _mm256_cvtepu16_epi32(b_part);
		partial_sums = _mm256_add_epi32(_mm256_mullo_epi32(a_part_converted, b_part_converted), partial_sums);
	}
	unsigned int extracted_partial_sums[8];
	_mm256_storeu_si256((__m256i*) &extracted_partial_sums, partial_sums);
	return extracted_partial_sums[0] + extracted_partial_sums[1] + extracted_partial_sums[2] + 
		extracted_partial_sums[3] + extracted_partial_sums[4] + extracted_partial_sums[5] +
		extracted_partial_sums[6] + extracted_partial_sums[7];
}

/* This is the list of functions to test */
function_info functions[] = {
    {dot_product_C, "C (local)"},
    {dot_product_gcc7_O3, "C (compiled with GCC7.2 -O3 -mavx2)"},
    {dot_product_AVX, "dot_product_AVX"},
    {NULL, NULL},
};
