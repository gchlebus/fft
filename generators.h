#ifndef GENERATORS_H
#define GENERATORS_H

typedef struct sig_param_ 
{
	double amp;
	double freq;
} sig_param;

typedef void generator_func(double *input, const unsigned int length, const double f_sampling, const sig_param param);

typedef struct generator_
{
	generator_func *func;
	sig_param param;
} generator;

void sine_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param);

void cosine_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param);

void rectangle_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param);

void white_noise_generator(double *input, const unsigned int length, const double f_sampling, const sig_param param);

int signal_generator (double **signal, const unsigned int length, const double f_sampling, const unsigned int n, const generator *generators);

#endif
