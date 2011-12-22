#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"

int main(void)
{
	int input[] = {1, 0, -1, 0, 0, 0, 0, 0};

	fft_to_frequency_domain(input, 8, 10, "fft2.csv");

	return 0;
}
