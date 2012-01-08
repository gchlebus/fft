#define _USE_MATH_DEFINES
#define _COMPLEX_DEFINED
#include "generators.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FFT.h"

void sine_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param)
{
	double t;
	unsigned int i;

	assert(input != NULL);

	for(i = 0; i < length; ++i)
	{
		t = i * ((double)1 / f_sampling);
		input[i].Re += param.amp * sin(2 * M_PI * param.freq * t);
	}
}

void cosine_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param)
{
	double t;
	unsigned int i;

	assert(input != NULL);

	for(i = 0; i < length; ++i)
	{
		t = i * ((double)1 / f_sampling);
		input[i].Re += param.amp * cos(2 * M_PI * param.freq * t);
	}
}

void rectangle_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param)
{
	double t;
	unsigned int i, j;

	assert(input != NULL);

	t = j = 0;
	for(i = 0; i < length; ++i)
	{
		if(t >= 1 / param.freq)
		{
			j = 0;
		}
		
		t = j++ * ((double)1 / f_sampling);	
		input[i].Re += t >= 0.5 * (1 / param.freq) ? -param.amp : param.amp;
	}
}

void white_noise_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param)
{
	unsigned int i;

	assert(input != NULL);

	for(i = 0; i < length; ++i)
	{
		input[i].Re += 2 * param.amp *  ((double)rand()/RAND_MAX - 0.5);
	}
}

int signal_generator(complex **signal, const unsigned int length, const double f_sampling, const unsigned int n, const generator *generators)
{
	unsigned int i;

	assert(generators != NULL);

	*signal = (complex *)calloc(length, sizeof(complex));
	if(*signal == NULL)
	{
		return 1; //allocation failed
	}

	for(i = 0; i < n; ++i)
	{
		(generators + i)->func(*signal, length, f_sampling, (generators + i)->param);
	}

	return 0;
}

