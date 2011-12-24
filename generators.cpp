#define _USE_MATH_DEFINES
#include "generators.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

void sine_generator(double *input, sig_param param)
{
	int i;
	double t;

	assert(input != NULL);

	for(i = 0; i < param.length; ++i)
	{
		t = i * ((double)1 / param.f_sampling);
		input[i] += param.amp * sin(2 * M_PI * param.freq * t);
	}
}

void cosine_generator(double *input, sig_param param)
{
	int i;
	double t;

	assert(input != NULL);

	for(i = 0; i < param.length; ++i)
	{
		t = i * ((double)1 / param.f_sampling);
		input[i] += param.amp * cos(2 * M_PI * param.freq * t);
	}
}

void rectangular_generator(double *input, sig_param param)
{
	int i, j;
	double t;

	assert(input != NULL);

	t = 0;
	j = 0;
	for(i = 0; i < param.length; ++i)
	{
		if(t >= 1 / param.freq)
		{
			j = 0;
		}
		
		t = j++ * ((double)1 / param.f_sampling);	
		input[i] += t >= 0.5 * (1 / param.freq) ? -param.amp : param.amp;
	}
}