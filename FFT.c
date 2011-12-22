#define _COMPLEX_DEFINED
#define _USE_MATH_DEFINES
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

void fft_bit_reverse_copy(const int *input, complex *output, int bits)
{
	int i;

	assert(input != NULL && output != NULL && bits > 0);

	for(i = 0; i < 1<<bits; i++)
	{
		output[reverse(i, bits)].Re = input[i];
	}
}

complex multiply(double num, complex B)
{
	complex A = {cos(num), sin(num)};
	complex ret;

	ret.Re = A.Re * B.Re - A.Im * B.Im;
	ret.Im = A.Re * B.Im + A.Im * B.Re;

	return ret;
}

complex * fft_iterative(const int *input, int bits)
{
	int s, k, j, m;
	double unity_root, w;
	complex *output, t, u;

	output = (complex *)calloc(1<<bits, sizeof(complex));
	if(output == NULL)
	{
		exit(EXIT_FAILURE);
	}
	fft_bit_reverse_copy(input, output, bits);

	for(s = 1; s <= bits; s++)
	{
		m = 1<<s;
		unity_root = (2 * M_PI) / (1<<bits);

		for(k = 0; k < 1<<bits; k += m)
		{
			w = 1;
			for(j = 0; j <= m/2-1; j++)
			{
				t = multiply((unity_root * j), output[k + j + m/2]);
				u = output[k+j];
				output[k+j].Re = u.Re + t.Re;
				output[k+j].Im = u.Im + t.Im;
				output[k + j + m/2].Re = u.Re - t.Re;
				output[k + j + m/2].Im = u.Im - t.Im;
			}
		}
	}
	return output;
}

