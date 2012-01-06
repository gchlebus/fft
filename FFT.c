#define _COMPLEX_DEFINED
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "FFT.h"

complex complex_multiply(complex A, complex B)
{
	complex ret;

	ret.Re = A.Re * B.Re - A.Im * B.Im;
	ret.Im = A.Re * B.Im + A.Im * B.Re;

	return ret;
}

unsigned int next_pow_2(unsigned int num)
{
	num--;
	num |= num >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
	num |= num >> 2;   // and then or the results.
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	num++;     

	return num;
}

int reverse(unsigned int num, unsigned int bits)
{
	unsigned int i, j, output, *stack;

	assert(num >= 0 && bits >= 0);

	stack = (unsigned int *)malloc(bits * sizeof(int));
	if(stack == NULL)
	{
		return -1;
	}

	i = 0;
	while(i != bits)
	{
		stack[i++] = num % 2;
		num /= 2;
	}

	output = 0;
	for(j = 0; i != 0; ++j)
	{
		output += stack[--i] * 1<<j;
	}

	free(stack);
	return output;
}

int fft_bit_reverse_copy(const double *input, complex *output, const unsigned int bits)
{
	unsigned int i, rev;

	assert(input != NULL && output != NULL && bits >= 0);

	for(i = 0; i < 1<<bits; ++i)
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

int fft_iterative(const double *input, complex *output, const unsigned int bits)
{
	double unity_root, w;
	unsigned int s, k, j, m;
	complex t, u, num;

	assert(input != NULL && output != NULL && bits >= 0);

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
				
				output[k + j].Re = u.Re + t.Re;
				output[k + j].Im = u.Im + t.Im;
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

int fft_to_frequency_domain(double **input, complex **fft, const unsigned int length)
{
	double *tmp;
	unsigned int bits, next_pow_2_num, i;

	assert(*input != NULL);

	next_pow_2_num = next_pow_2(length);
	if(next_pow_2_num != length)
	{
		tmp = (double *)realloc(*input, next_pow_2_num * sizeof(double));
		if(tmp == NULL)
		{
			return -1; //memory reallocation failed
		}
		else
		{
			*input = tmp;
			for(i = length; i < next_pow_2_num; ++i)
			{
				(*input)[i] = 0;
			}
		}
	}

	*fft = (complex *)calloc(next_pow_2_num, sizeof(complex));
	if(*fft == NULL)
	{
		return -1; //memory allocation failed
	}
	
	for(bits = 0; (next_pow_2_num - 1)>>bits; ++bits);
	//bits = (int) (log10((float)next_pow_2_num) / log10((float)2)); //log2(length)

	if(fft_iterative(*input, *fft, bits))
	{
		return -1; //fft_iterative failed
	}

	return 0;
}

double fft_get_main_frequency(const complex *fft, const unsigned int length, const double f_sampling)
{
	double freq, main_freq, tmp_value, max_value;
	unsigned int i;

	assert(fft != NULL && f_sampling > 0);

	max_value = main_freq = 0;
	for(i = 0; i < length / 2; ++i)
	{
		freq = i * ((double)f_sampling / length);
		tmp_value = sqrt(pow(fft[i].Re,2) + pow(fft[i].Im,2));

		if(tmp_value > max_value)
		{
			max_value = tmp_value;
			main_freq = freq;
		}
	}

	return main_freq;
}
