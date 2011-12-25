#ifndef FFT_H
#define FFT_H

typedef struct _complex
{
	double Re, Im;
} complex;

int reverse(unsigned int num, unsigned int bits);

int fft_bit_reverse_copy(const double *input, complex *output, unsigned int bits);

int fft_iterative(const double *input, complex *output, unsigned int bits);

int fft_to_frequency_domain(const double *input, complex **fft, unsigned int length, double f_sampling);

int fft_get_main_frequency(double *main_freq, const complex *fft, unsigned int length, double f_sampling);

#endif //FFT_H