#ifndef FFT_H
#define FFT_H

typedef struct _complex
{
	double Re, Im;
} complex;

int reverse(int num, int bits);

int fft_bit_reverse_copy(const int *input, complex *output, int bits);

int fft_iterative(const int *input, complex *output, int bits);

int fft_to_frequency_domain(const int *input, int length, int f_sampling, const char *filename);

int fft_get_input(const char *filename);

#endif //FFT_H