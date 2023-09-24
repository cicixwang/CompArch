#include <stdlib.h>
#include <limits.h>  /* for USHRT_MAX */

#include <immintrin.h>

#include "min.h"
/* reference implementation in C */
short min_C(long size, short * a) {
    short result = SHRT_MAX;
    for (int i = 0; i < size; ++i) {
        if (a[i] < result)
            result = a[i];
    }
    return result;
}

short min_AVX(long size, short * a) {
    	short result = SHRT_MAX;
	__m256i partial_results = _mm256_set1_epi16(SHRT_MAX);
	for (int i=0; i<size; i+=16) {
		__m256i a_part = _mm256_loadu_si256((__m256i*) &a[i]);
		partial_results = _mm256_min_epi16(a_part, partial_results);
	}
	short extracted_partial_results[16];
 	_mm256_storeu_si256((__m256i*) & extracted_partial_results, partial_results);
	for (int i=0; i<16; i++) {
		if (extracted_partial_results[i] < result)
			result = extracted_partial_results[i];
	}
	return result;
}


/* This is the list of functions to test */
function_info functions[] = {
    {min_C, "C (local)"},
    {min_AVX, "min_AVX"},
    {NULL, NULL},
};
