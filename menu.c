#define _COMPLEX_DEFINED
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "menu.h"
#include "generators.h"
#include "FFT.h"

double *signal;
unsigned int n_samples;
double f_sampling;
complex *fft;

static void print_string(const char *str)
{
	while(*str != NULL)
	{
		printf("%c", *(str++));
	}
}

static long int uint_get_user_input(const char *msg,  const char * warn, unsigned int lbound, unsigned int rbound)
{
	char *c, str[20];
	unsigned int input;
	
	while(1)
	{
		print_string(msg);
		gets(str);
		input = strtol(str, &c, 10);
		if(*c == NULL && input >= lbound && input <= rbound)
		{
			printf("\n");
			return input;
		}
		else if(*c != NULL)
		{
			printf("Wprowadzono nieprawid³owy format danych!\n");
		}
		else
		{
			print_string(warn);
		}
	}
}

static double dbl_get_user_input(const char* msg, const char *warn, double lbound, double rbound)
{
	char *c, str[20];
	double input;

	while(1)
	{
		print_string(msg);
		gets(str);
		input = strtod(str, &c);
		if(*c == NULL && input >= lbound && input <= rbound)
		{
			printf("\n");
			return input;
		}
		else if(*c != NULL)
		{
			printf("Wprowadzono nieprawid³owy format danych!\n");
		}
		else
		{
			print_string(warn);
		}
	}
}

void menu(void)
{
	while(1)
	{
		main_menu();
		switch(uint_get_user_input("Wybierz opcjê: ", "Nie ma takiej opcji!\n", 1, 7))
		{
		case 1:
			generate_signal();
			break;
		case 2:
			//load_signal_from_file();
			break;
		case 3:
			save_signal_to_file();
			break;
		case 4:
			calculate_fft();
			break;
		case 5:
			save_fft_to_file();
			break;
		case 6:
			get_main_frequency();
			break;
		case 7:
			end();
			return;
		}
	}
}

static void main_menu(void)
{
	printf("Menu\n");
	printf("1. Wygeneruj sygna³\n");
	printf("2. Wczytaj sygna³ z pliku\n");
	printf("3. Zapisz sygna³ do pliku\n");
	printf("4. Oblicz FFT\n");
	printf("5. Zapisz FFT sygna³u do pliku\n");
	printf("6. Oblicz g³ówn¹ sk³adow¹ czêstotliwoœciow¹\n");
	printf("7. Zakoñcz\n");
}

void generate_signal_menu(void)
{
	printf("Wybierz rodzaj sygna³u:\n");
	printf("1. Sinus\n");
	printf("2. Cosinus\n");
	printf("3. Prostok¹t (symetryczny)\n");
	printf("4. Trójk¹t (symetryczny)\n");
	printf("5. Bia³y szum\n");
	printf("6. Gotowe\n");
}

static int generate_signal(void)
{
	unsigned int n_funcs = 0;
	generator_func *func;
	generator *tmp, *generators = NULL;

	n_samples = uint_get_user_input("Podaj iloœæ próbek: ", "B³êdna iloœæ próbek!\n", 1, UINT_MAX);
	f_sampling = dbl_get_user_input("Podaj czêstotliwoœæ próbkowania sygna³u[Hz]: ", "B³êdna czêstotliwoœæ!\n", 1.0/INT_MAX, INT_MAX);

	while(1)
	{
		generate_signal_menu();
		switch(uint_get_user_input("Wybierz opcjê: ", "Nie ma takiej opcji\n", 1, 6))
		{
		case 1:
			func = sine_generator;
			break;
		case 2:
			func = cosine_generator;
			break;
		case 3:
			func = rectangle_generator;
			break;
		case 4:
			func = triangle_generator;
			break;
		case 5:
			func = white_noise_generator;
			break;
		case 6:
			signal_generator(&signal, n_samples, f_sampling, n_funcs, generators);
			return 0;
		}

		tmp = (generator *)realloc(generators, (n_funcs+1) * sizeof(generator));
		if(tmp == NULL)
		{
			if(generators != NULL)
			{
				free(generators);
			}
			return -1;
		}

		generators = tmp;
		generators[n_funcs].func = func;
		generators[n_funcs].param.amp = dbl_get_user_input("Podaj amplitudê: ", "B³êdna wartoœæ!\n", 0, INT_MAX);
		generators[n_funcs].param.freq = dbl_get_user_input("Podaj czêstotliwoœæ[Hz]: ", "B³êdna wartoœæ!\n", 0, INT_MAX);
		n_funcs++;

		if(fft != NULL)
		{
			printf("FFT poprzedniego sygna³u zosta³o wykasowane\n\n");
			free(fft);
			fft = NULL;
		}
	}
}

static void save_signal_to_file(void)
{
	unsigned int i;
	char filename[100];
	double t;
	FILE *pFile;

	if(signal != NULL)
	{
		printf("Podaj nazwê pliku: ");
		gets(filename);
		pFile = fopen(filename, "w");
		if(pFile != NULL)
		{
			fprintf(pFile, "Time [s]; y(t)\n"); //header
			for(i = 0; i < next_pow_2(n_samples); ++i)
			{
				t = i * (1 / f_sampling);
				fprintf(pFile, "%.4lf;%.4lf\n", t, signal[i]);
			}
			fclose(pFile);
		}
		else
		{
			printf("B³¹d podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("W pamiêci nie ma ¿adnego sygna³u!\n\n");
	}
}

static void end(void)
{
	if(signal != NULL)
	{
		free(signal);
	}

	if(fft != NULL)
	{
		free(fft);
	}
}

static void calculate_fft(void)
{
	if(signal != NULL)
	{
		fft_to_frequency_domain(signal, &fft, next_pow_2(n_samples), f_sampling);
	}
	else
	{
		printf("W pamiêci nie ma ¿adnego sygna³u\n\n");
	}
}

static void save_fft_to_file(void)
{
	unsigned int i, length;
	char filename[100];
	double freq;
	FILE *pFile;

	if(fft != NULL)
	{
		printf("Podaj nazwê pliku: ");
		gets(filename);
		pFile = fopen(filename, "w");
		if(pFile != NULL)
		{
			length = next_pow_2(n_samples);
			fprintf(pFile, "Frequency [Hz]; |FFT|\n"); //header
			for(i = 0; i < next_pow_2(n_samples); ++i)
			{
				freq = i * ((double)f_sampling / length);
				fprintf(pFile, "%.4lf;%.4lf\n", freq, (sqrt(pow(fft[i].Re,2) + pow(fft[i].Im,2)))/length);
			}
			fclose(pFile);
		}
		else
		{
			printf("B³¹d podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("W pamiêci nie ma ¿adnego fft\n\n");
	}
}

static void get_main_frequency(void)
{
	double main_freq;

	if(fft != NULL)
	{
		fft_get_main_frequency(&main_freq, fft, next_pow_2(n_samples), f_sampling);
		printf("G³ówna sk³adowa to: %.4lf\n", main_freq);
	}
	else
	{
		printf("W pamiêci nie ma ¿adnego fft\n\n");
	}
}