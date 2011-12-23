#ifndef FFT_H
#define FFT_H

typedef struct _complex
{
	double Re, Im;
} complex;

int reverse(int num, int bits);

int fft_bit_reverse_copy(const double *input, complex *output, int bits);

int fft_iterative(const double *input, complex *output, int bits);

int fft_to_frequency_domain(const double *input, int length, int f_sampling, const char *filename);

int fft_get_main_frequency(double *main_freq, const double *input, int length, int f_sampling);

void sine_generator(double *input, double amp, double sine_freq, double noise_amp, int length, int f_sampling);

#endif //FFT_H