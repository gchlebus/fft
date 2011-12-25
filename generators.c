#define _USE_MATH_DEFINES
#include "generators.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

void sine_generator(double *input, unsigned int length, double f_sampling, sig_param param)
{
	unsigned int i;
	double t;

	assert(input != NULL);

	for(i = 0; i < length; ++i)
	{
		t = i * ((double)1 / f_sampling);
		input[i] += param.amp * sin(2 * M_PI * param.freq * t);
	}
}

void cosine_generator(double *input, unsigned int length, double f_sampling, sig_param param)
{
	unsigned int i;
	double t;

	assert(input != NULL);

	for(i = 0; i < length; ++i)
	{
		t = i * ((double)1 / f_sampling);
		input[i] += param.amp * cos(2 * M_PI * param.freq * t);
	}
}

void rectangle_generator(double *input, unsigned int length, double f_sampling, sig_param param)
{
	unsigned int i, j;
	double t;

	assert(input != NULL);

	t = 0;
	j = 0;
	for(i = 0; i < length; ++i)
	{
		if(t >= 1 / param.freq)
		{
			j = 0;
		}
		
		t = j++ * ((double)1 / f_sampling);	
		input[i] += t >= 0.5 * (1 / param.freq) ? -param.amp : param.amp;
	}
}

void triangle_generator(double *input, unsigned int length, double f_sampling, sig_param param)
{

}

void white_noise_generator(double *input, unsigned int length, double f_sampling, sig_param param)
{
	unsigned int i;

	assert(input != NULL);

	srand(time(NULL));

	for(i = 0; i < length; ++i)
	{
		input[i] += 2 * param.amp *  ((double)rand()/RAND_MAX - 0.5);
	}
}

int signal_generator(double **signal, unsigned int length, double f_sampling, unsigned int n, const generator *generators)
{
	unsigned int i;
	double t;

	assert(generators != NULL);

	*signal = (double *)calloc(next_pow_2(length), sizeof(double));
	if(*signal == NULL)
	{
		return -1; //allocation failed
	}

	for(i = 0; i < n; ++i)
	{
		(generators + i)->func(*signal, length, f_sampling, (generators + i)->param);
	}

	if(length != next_pow_2(length))
	{
		printf("Na koñcu sygna³u zosta³y dodane zera!\n\n");
	}

	return 0;
}

