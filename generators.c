#define _USE_MATH_DEFINES
#include "generators.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void sine_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param)
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

void cosine_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param)
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

void rectangle_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param)
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

void white_noise_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param)
{
	unsigned int i;

	assert(input != NULL);

	srand(time(NULL));

	for(i = 0; i < length; ++i)
	{
		input[i] += 2 * param.amp *  ((double)rand()/RAND_MAX - 0.5);
	}
}

int signal_generator(double **signal, const unsigned int length, const double f_sampling, const unsigned int n, const generator *generators)
{
	unsigned int i;

	assert(generators != NULL);

	*signal = (double *)calloc(length, sizeof(double));
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

