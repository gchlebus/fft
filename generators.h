#ifndef GENERATORS_H
#define GENERATORS_H

typedef struct sig_param_ 
{
	double amp;
	double freq;
} sig_param;

typedef void generator_func(double *input, unsigned int length, double f_sampling, sig_param param);

typedef struct generator_
{
	generator_func *func;
	sig_param param;
} generator;

unsigned int next_pow_2(unsigned int num);

void sine_generator(double *input, unsigned int length, double f_sampling, sig_param param);

void cosine_generator(double *input, unsigned int length, double f_sampling, sig_param param);

void rectangle_generator(double *input, unsigned int length, double f_sampling, sig_param param);

void triangle_generator(double *input, unsigned int length, double f_sampling, sig_param param);

void white_noise_generator(double *input, unsigned int length, double f_sampling, sig_param param);

int signal_generator (double **signal, unsigned int length, double f_sampling, unsigned int n, const generator *generators);

#endif
