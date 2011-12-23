#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"

#define SIZE 1024

int main(void)
{
	const int length = SIZE;
	const int f_sampling = 1024;
	double input[SIZE];
	double main_freq;

	sine_generator(input, 2, 50, 1.5, length, f_sampling);

	fft_to_frequency_domain(input, length, f_sampling, "fft.csv");
	fft_get_main_frequency(&main_freq, input, length, f_sampling);

	printf("Main frequency = %.2lf Hz\n", main_freq);

	return 0;
}
