#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"

int main(void)
{
	int i;
	int bits = 3;
	int input[] = {1, 0, -1, 0, 1, 0, -1, 0};
	complex *output;
	output = fft_iterative(input, bits);

	for(i = 0; i < 1<<bits; i++)
	{
		printf("%d. Re = %.2lf; Im = %.2lf\n", i, output[i].Re, output[i].Im);
	}
	free(output);
	return 0;
}
