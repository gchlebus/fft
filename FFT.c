#define _COMPLEX_DEFINED
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "FFT.h"

int reverse(unsigned int num, unsigned int bits)
{
	int *stack = NULL, *tmp;
	int height = 0, max_heigth = 0;
	unsigned int i;
	unsigned int output = 0;

	assert(num >= 0 && bits > 0);

	while(num != 0 || height != bits)
	{
		if(height >= max_heigth)
		{
			max_heigth = 2 * height + 2;
			tmp = (int *)realloc(stack, max_heigth * sizeof(int));
			if(tmp == NULL)
			{
				free(stack);
				return -1;
			}
			stack = tmp;
		}

		stack[height++] = num % 2;
		num /= 2;
	}

	for(i = 0; height != 0; i++)
	{
		output += stack[--height] * 1<<i;
	}

	free(stack);
	return output;
}

int fft_bit_reverse_copy(const double *input, complex *output, unsigned int bits)
{
	unsigned int i, rev;

	assert(input != NULL && output != NULL && bits > 0);

	for(i = 0; i < 1<<bits; i++)
	{
		rev = reverse(i, bits);
		if(rev == -1)
		{
			return -1; //reverse function failed
		}

		output[rev].Re = input[i];
	}
	return 0;
}

complex complex_multiply(complex A, complex B)
{
	complex ret;

	ret.Re = A.Re * B.Re - A.Im * B.Im;
	ret.Im = A.Re * B.Im + A.Im * B.Re;

	return ret;
}

int fft_iterative(const double *input, complex *output, unsigned int bits)
{
	unsigned int s, k, j, m;
	double unity_root, w;
	complex t, u;
	complex num;

	assert(input != NULL && output != NULL && bits > 0);

	if(fft_bit_reverse_copy(input, output, bits) == -1)
	{
		return -1; //fft_bit_reverse_copy failed
	}

	for(s = 1; s <= bits; s++)
	{
		m = 1<<s; //number of elements for which DFT is computed
		unity_root = -(2 * M_PI) / m; //exponent of e - expotential form of complex number

		for(k = 0; k < 1<<bits; k += m)
		{
			num.Re = 1;
			num.Im = 0;
			for(j = 0; j <= m/2-1; )
			{
				t = complex_multiply(num, output[k + j + m/2]);
				u = output[k+j];
				
				output[k+j].Re = u.Re + t.Re;
				output[k+j].Im = u.Im + t.Im;
				output[k + j + m/2].Re = u.Re - t.Re;
				output[k + j + m/2].Im = u.Im - t.Im;
				
				w = unity_root * (++j); 
				num.Re = cos(w);
				num.Im = sin(w);
			}
		}
	}

	return 0;
}

int fft_to_frequency_domain(const double *input, complex **fft, unsigned int length, double f_sampling)
{
	unsigned int bits;

	assert(input != NULL && length != 0 && f_sampling > 0);

	*fft = (complex *)calloc(length, sizeof(complex));
	if(*fft == NULL)
	{
		return -1; //memory allocation failed
	}

	bits = (int) (log10((float)length) / log10((float)2)); //log2(length)
	if(fft_iterative(input, *fft, bits) ==  -1)
	{
		return -1; //fft_iterative failed
	}

	return 0;
}

int fft_get_main_frequency(double *main_freq, const complex *fft, unsigned int length, double f_sampling)
{
	unsigned int i;
	double freq;
	double tmp, current_max = 0;

	assert(main_freq != NULL && fft != NULL);

	for(i = 0; i < length/2 + 1; ++i)
	{
		freq = i * ((double)f_sampling / length);
		tmp = sqrt(pow(fft[i].Re,2) + pow(fft[i].Im,2));

		if(tmp > current_max)
		{
			current_max = tmp;
			*main_freq = freq;
		}
	}

	return 0;
}
