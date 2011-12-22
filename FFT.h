#ifndef FFT_H
#define FFT_H

typedef struct _complex
{
	double Re, Im;
} complex;

int reverse(int num, int bits);

void fft_bit_reverse_copy(const int *input, complex *output, int bits);

complex * fft_iterative(const int *input, int bits);

#endif //FFT_H