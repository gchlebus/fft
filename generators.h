#ifndef GENERATORS_H
#define GENERATORS_H

typedef struct sig_param_ 
{
	double amp;
	double freq;
	double length;
	double f_sampling;
} sig_param;

typedef void generator_func(double *input, sig_param param);

typedef struct generator_
{
	generator_func *func;
	sig_param param;
} generator;

void sine_generator(double *input, sig_param param);

void cosine_generator(double *input, sig_param param);

void rectangular_generator(double *input, sig_param param);

void traingle_generator(double *input, sig_param param);

void white_noise_generator(double *input, sig_param param);

#endif
