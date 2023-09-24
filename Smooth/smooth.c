#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <immintrin.h>

/* 
 * Please fill in the following team struct 
 */
who_t who = {
    "High Pass",           /* Scoreboard name */

    "Xi Wang",      /* First member full name */
    "xw2ez@virginia.edu",     /* First member email address */
};

/*** UTILITY FUNCTIONS ***/

/* You are free to use these utility functions, or write your own versions
 * of them. */

/* A struct used to compute averaged pixel value */
typedef struct {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short alpha;
    unsigned short num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = sum->alpha = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->alpha += (int) p.alpha;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    current_pixel->alpha = (unsigned short) (sum.alpha/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(int jj=max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	for(int ii=max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	    accumulate_sum(&sum, src[RIDX(ii,jj,dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
 
    return current_pixel;
}


/******************************************************
 * Your different versions of the smooth go here
 ******************************************************/

/* 
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    for (int i = 0; i < dim; i++)
	for (int j = 0; j < dim; j++)
            dst[RIDX(i,j, dim)] = avg(dim, i, j, src);
}


char another_smooth_descr[] = "another_smooth: Another version of smooth";
void another_smooth(int dim, pixel *src, pixel *dst) 
{	
	// Corner Cases
	int cornerDim = dim-1;

	pixel_sum sum;

	pixel px;
    	pixel* current_pixel = &px;

    	initialize_pixel_sum(&sum);
	
	accumulate_sum(&sum, src[RIDX(0,0,dim)]);
	accumulate_sum(&sum, src[RIDX(1,0,dim)]);
	accumulate_sum(&sum, src[RIDX(0,1,dim)]);
	accumulate_sum(&sum, src[RIDX(1,1,dim)]);
	
	current_pixel->red = (unsigned short) (sum.red >> 2);
    	current_pixel->green = (unsigned short) (sum.green >> 2);
    	current_pixel->blue = (unsigned short) (sum.blue >> 2);
    	current_pixel->alpha = (unsigned short) (sum.alpha >> 2);
	dst[RIDX(0,0, dim)] = px;

	initialize_pixel_sum(&sum);	
	
	accumulate_sum(&sum, src[RIDX(cornerDim,0,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim-1,0,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim,1,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim-1,1,dim)]);

	current_pixel->red = (unsigned short) (sum.red >> 2);
    	current_pixel->green = (unsigned short) (sum.green >> 2);
    	current_pixel->blue = (unsigned short) (sum.blue >> 2);
    	current_pixel->alpha = (unsigned short) (sum.alpha >> 2);
	dst[RIDX(cornerDim,0, dim)] = px;

	initialize_pixel_sum(&sum);

	accumulate_sum(&sum, src[RIDX(0,cornerDim,dim)]);
	accumulate_sum(&sum, src[RIDX(1,cornerDim,dim)]);
	accumulate_sum(&sum, src[RIDX(0,cornerDim-1,dim)]);
	accumulate_sum(&sum, src[RIDX(1,cornerDim-1,dim)]);

	current_pixel->red = (unsigned short) (sum.red >> 2);
    	current_pixel->green = (unsigned short) (sum.green >> 2);
    	current_pixel->blue = (unsigned short) (sum.blue >> 2);
    	current_pixel->alpha = (unsigned short) (sum.alpha >> 2);
	dst[RIDX(0, cornerDim, dim)] = px;
	
	initialize_pixel_sum(&sum);

	accumulate_sum(&sum, src[RIDX(cornerDim,cornerDim,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim-1,cornerDim,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim,cornerDim-1,dim)]);
	accumulate_sum(&sum, src[RIDX(cornerDim-1,cornerDim-1,dim)]);

	current_pixel->red = (unsigned short) (sum.red >> 2);
    	current_pixel->green = (unsigned short) (sum.green >> 2);
    	current_pixel->blue = (unsigned short) (sum.blue >> 2);
    	current_pixel->alpha = (unsigned short) (sum.alpha >> 2);
	dst[RIDX(cornerDim, cornerDim, dim)] = px;

	// Edge Cases
	for (int j=1; j<dim-1; j++) {
	
    		initialize_pixel_sum(&sum); 

	    	accumulate_sum(&sum, src[RIDX(0,j,dim)]);
		accumulate_sum(&sum, src[RIDX(0,j-1,dim)]);
		accumulate_sum(&sum, src[RIDX(0,j+1,dim)]);
		accumulate_sum(&sum, src[RIDX(1,j,dim)]);
		accumulate_sum(&sum, src[RIDX(1,j-1,dim)]);
		accumulate_sum(&sum, src[RIDX(1,j+1,dim)]);

		current_pixel->red = (unsigned short) (sum.red/6);
    		current_pixel->green = (unsigned short) (sum.green/6);
    		current_pixel->blue = (unsigned short) (sum.blue/6);
    		current_pixel->alpha = (unsigned short) (sum.alpha/6);

		dst[RIDX(0,j, dim)] = px;
	}
	for (int j=1; j<dim-1; j++) {
	
    		initialize_pixel_sum(&sum); 

	    	accumulate_sum(&sum, src[RIDX(cornerDim,j,dim)]);
		accumulate_sum(&sum, src[RIDX(cornerDim,j-1,dim)]);
		accumulate_sum(&sum, src[RIDX(cornerDim,j+1,dim)]);
		accumulate_sum(&sum, src[RIDX(cornerDim-1,j,dim)]);
		accumulate_sum(&sum, src[RIDX(cornerDim-1,j-1,dim)]);
		accumulate_sum(&sum, src[RIDX(cornerDim-1,j+1,dim)]);

		current_pixel->red = (unsigned short) (sum.red/6);
    		current_pixel->green = (unsigned short) (sum.green/6);
    		current_pixel->blue = (unsigned short) (sum.blue/6);
    		current_pixel->alpha = (unsigned short) (sum.alpha/6);

		dst[RIDX(cornerDim,j, dim)] = px;
	}
	for (int i=1; i<dim-1; i++) {
	
    		initialize_pixel_sum(&sum); 

	    	accumulate_sum(&sum, src[RIDX(i,0,dim)]);
		accumulate_sum(&sum, src[RIDX(i-1,0,dim)]);
		accumulate_sum(&sum, src[RIDX(i+1,0,dim)]);
		accumulate_sum(&sum, src[RIDX(i,1,dim)]);
		accumulate_sum(&sum, src[RIDX(i-1,1,dim)]);
		accumulate_sum(&sum, src[RIDX(i+1,1,dim)]);

		current_pixel->red = (unsigned short) (sum.red/6);
    		current_pixel->green = (unsigned short) (sum.green/6);
    		current_pixel->blue = (unsigned short) (sum.blue/6);
    		current_pixel->alpha = (unsigned short) (sum.alpha/6);

		dst[RIDX(i,0, dim)] = px;
	}
	for (int i=1; i<dim-1; i++) {
	
    		initialize_pixel_sum(&sum); 	

	    	accumulate_sum(&sum, src[RIDX(i,cornerDim,dim)]);
		accumulate_sum(&sum, src[RIDX(i-1,cornerDim,dim)]);
		accumulate_sum(&sum, src[RIDX(i+1,cornerDim,dim)]);
		accumulate_sum(&sum, src[RIDX(i,cornerDim-1,dim)]);
		accumulate_sum(&sum, src[RIDX(i-1,cornerDim-1,dim)]);
		accumulate_sum(&sum, src[RIDX(i+1,cornerDim-1,dim)]);

		current_pixel->red = (unsigned short) (sum.red/6);
    		current_pixel->green = (unsigned short) (sum.green/6);
    		current_pixel->blue = (unsigned short) (sum.blue/6);
    		current_pixel->alpha = (unsigned short) (sum.alpha/6);

		dst[RIDX(i,cornerDim, dim)] = px;
	}

	// Middle Cases
	for (int i = 1; i < dim-1; i++) {
		for (int j = 1; j < dim-3; j+=4) {
			
			//initialize_pixel_sum(&sum);			
			
			__m128i the_pixel1 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j, dim)]);
			__m256i the_pixel_converted1 = _mm256_cvtepu8_epi16(the_pixel1);
			__m128i the_pixel2 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j-1, dim)]);
			__m256i the_pixel_converted2 = _mm256_cvtepu8_epi16(the_pixel2);
			__m128i the_pixel3 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j+1, dim)]);
			__m256i the_pixel_converted3 = _mm256_cvtepu8_epi16(the_pixel3);
			__m128i the_pixel4 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j, dim)]);
			__m256i the_pixel_converted4 = _mm256_cvtepu8_epi16(the_pixel4);
			__m128i the_pixel5 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j, dim)]);
			__m256i the_pixel_converted5 = _mm256_cvtepu8_epi16(the_pixel5);
			__m128i the_pixel6 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j-1, dim)]);
			__m256i the_pixel_converted6 = _mm256_cvtepu8_epi16(the_pixel6);
			__m128i the_pixel7 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j+1, dim)]);
			__m256i the_pixel_converted7 = _mm256_cvtepu8_epi16(the_pixel7);
			__m128i the_pixel8 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j-1, dim)]);
			__m256i the_pixel_converted8 = _mm256_cvtepu8_epi16(the_pixel8);
			__m128i the_pixel9 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j+1, dim)]);
			__m256i the_pixel_converted9 = _mm256_cvtepu8_epi16(the_pixel9);

			__m256i sum_of_pixels = _mm256_setzero_si256();
		
			sum_of_pixels = _mm256_add_epi16(the_pixel_converted1, _mm256_add_epi16(the_pixel_converted2, _mm256_add_epi16(the_pixel_converted3, 
				_mm256_add_epi16(the_pixel_converted4, _mm256_add_epi16(the_pixel_converted5, _mm256_add_epi16(the_pixel_converted6, 
				_mm256_add_epi16(the_pixel_converted7, _mm256_add_epi16(the_pixel_converted8, the_pixel_converted9))))))));

			unsigned short pixel_elements[16];
			_mm256_storeu_si256((__m256i*) pixel_elements, sum_of_pixels);
						
			current_pixel->red = (unsigned short) ((pixel_elements[0] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[1] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[2] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[3] * 7282) >> 16);
			dst[RIDX(i,j, dim)] = px;

			//initialize_pixel_sum(&sum);
			current_pixel->red = (unsigned short) ((pixel_elements[4] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[5] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[6] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[7] * 7282) >> 16);
			dst[RIDX(i,j+1, dim)] = px;

			//initialize_pixel_sum(&sum);
			current_pixel->red = (unsigned short) ((pixel_elements[8] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[9] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[10] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[11] * 7282) >> 16);
			dst[RIDX(i,j+2, dim)] = px;

			//initialize_pixel_sum(&sum);
			current_pixel->red = (unsigned short) ((pixel_elements[12] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[13] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[14] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[15] * 7282) >> 16);
			dst[RIDX(i,j+3, dim)] = px;			
		}
	}
	for (int i = 1; i < dim-1; i++) {
		for (int j = dim-3; j < dim-1; j+=2) {
			
			//initialize_pixel_sum(&sum);			
			
			__m128i the_pixel1 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j, dim)]);
			__m256i the_pixel_converted1 = _mm256_cvtepu8_epi16(the_pixel1);
			__m128i the_pixel2 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j-1, dim)]);
			__m256i the_pixel_converted2 = _mm256_cvtepu8_epi16(the_pixel2);
			__m128i the_pixel3 = _mm_loadu_si128((__m128i*) &src[RIDX(i, j+1, dim)]);
			__m256i the_pixel_converted3 = _mm256_cvtepu8_epi16(the_pixel3);
			__m128i the_pixel4 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j, dim)]);
			__m256i the_pixel_converted4 = _mm256_cvtepu8_epi16(the_pixel4);
			__m128i the_pixel5 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j, dim)]);
			__m256i the_pixel_converted5 = _mm256_cvtepu8_epi16(the_pixel5);
			__m128i the_pixel6 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j-1, dim)]);
			__m256i the_pixel_converted6 = _mm256_cvtepu8_epi16(the_pixel6);
			__m128i the_pixel7 = _mm_loadu_si128((__m128i*) &src[RIDX(i-1, j+1, dim)]);
			__m256i the_pixel_converted7 = _mm256_cvtepu8_epi16(the_pixel7);
			__m128i the_pixel8 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j-1, dim)]);
			__m256i the_pixel_converted8 = _mm256_cvtepu8_epi16(the_pixel8);
			__m128i the_pixel9 = _mm_loadu_si128((__m128i*) &src[RIDX(i+1, j+1, dim)]);
			__m256i the_pixel_converted9 = _mm256_cvtepu8_epi16(the_pixel9);

			__m256i sum_of_pixels = _mm256_setzero_si256();
		
			sum_of_pixels = _mm256_add_epi16(the_pixel_converted1, _mm256_add_epi16(the_pixel_converted2, _mm256_add_epi16(the_pixel_converted3, 
				_mm256_add_epi16(the_pixel_converted4, _mm256_add_epi16(the_pixel_converted5, _mm256_add_epi16(the_pixel_converted6, 
				_mm256_add_epi16(the_pixel_converted7, _mm256_add_epi16(the_pixel_converted8, the_pixel_converted9))))))));

			unsigned short pixel_elements[16];
			_mm256_storeu_si256((__m256i*) pixel_elements, sum_of_pixels);
						
			current_pixel->red = (unsigned short) ((pixel_elements[0] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[1] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[2] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[3] * 7282) >> 16);
			dst[RIDX(i,j, dim)] = px;

			//initialize_pixel_sum(&sum);
			current_pixel->red = (unsigned short) ((pixel_elements[4] * 7282) >> 16);
    			current_pixel->green = (unsigned short) ((pixel_elements[5] * 7282) >> 16);
    			current_pixel->blue = (unsigned short) ((pixel_elements[6] * 7282) >> 16);
    			current_pixel->alpha = (unsigned short) ((pixel_elements[7] * 7282) >> 16);
			dst[RIDX(i,j+1, dim)] = px;
			
		}
	}
	

}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth function by calling the add_smooth_function() for
 *     each test function. When you run the benchmark program, it will
 *     test and report the performance of each registered test
 *     function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&another_smooth, another_smooth_descr);
}
