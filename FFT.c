#define _COMPLEX_DEFINED
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "FFT.h"

int reverse(int num, int bits)
{
	int *stack = NULL, *tmp;
	int height = 0, max_heigth = 0;
	int i;
	int output = 0;

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

int fft_bit_reverse_copy(const int *input, complex *output, int bits)
{
	int i, rev;

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

complex multiply(double num, complex B)
{
	complex ret;
	complex A = {cos(num), sin(num)};

	ret.Re = A.Re * B.Re - A.Im * B.Im;
	ret.Im = A.Re * B.Im + A.Im * B.Re;

	return ret;
}

int fft_iterative(const int *input, complex *output, int bits)
{
	int s, k, j, m;
	double unity_root, w;
	complex t, u;

	assert(input != NULL && output != NULL && bits > 0);

	if(fft_bit_reverse_copy(input, output, bits) == -1)
	{
		return -1; //fft_bit_reverse_copy failed
	}

	for(s = 1; s <= bits; s++)
	{
		m = 1<<s;
		unity_root = -(2 * M_PI) / (1<<bits);

		for(k = 0; k < 1<<bits; k += m)
		{
			w = 2 * M_PI; //¿eby w multiply z tego wysz³a jedynka
			for(j = 0; j <= m/2-1; )
			{
				t = multiply(w, output[k + j + m/2]);
				u = output[k+j];
				
				output[k+j].Re = u.Re + t.Re;
				output[k+j].Im = u.Im + t.Im;
				output[k + j + m/2].Re = u.Re - t.Re;
				output[k + j + m/2].Im = u.Im - t.Im;
				
				w = unity_root * (++j); //kolejne potêgi unity_root
			}
		}
	}
	return 0;
}

int fft_to_frequency_domain(const int *input, int length, int f_sampling, const char *filename)
{
	int bits, i;
	double freq;
	complex *output = NULL;
	FILE *pFile;

	assert(input != NULL && length != 0 && f_sampling > 0);

	output = (complex *)calloc(length, sizeof(complex));
	if(output == NULL)
	{
		return -1; //memory allocation failed
	}

	printf("%lf\n", log10((float)length)/log10((float)2));
	bits = (int) (log10((float)length) / log10((float)2)); //log2(length)

	if(fft_iterative(input, output, bits) ==  -1)
	{
		return -1; //fft_iterative failed
	}

	pFile = fopen(filename, "w");
	if(pFile != NULL)
	{
		fprintf(pFile, "Frequency [Hz]; |FFT|\n"); //header
		for(i = 0; i < length; ++i)
		{
			freq = (double)i * (f_sampling / length);
			output[i].Re = (sqrt(pow(output[i].Re,2) + pow(output[i].Im,2)))/length; //obliczanie modu³u i dzielenie przez length
			
			fprintf(pFile, "%.4lf;%.4lf\n", freq, output[i].Re);
		}
		fclose(pFile);
	}
	else
	{
		free(output);
		return -1; //file creation failed
	}

	free(output);
	return 0;
}

int fft_get_input(const char *filename)
{
	FILE *pFile;

	pFile = fopen(filename, "r");

}