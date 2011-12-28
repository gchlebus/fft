#define _COMPLEX_DEFINED
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "menu.h"
#include "generators.h"
#include "FFT.h"

double *signal;
complex *fft;
unsigned int n_samples;
double f_sampling;

static void print_string(const char *str)
{
	while(*str != NULL)
	{
		printf("%c", *(str++));
	}
}

static long int uint_get_user_input(const char *msg,  const char *warn, const long int lbound, const long int rbound)
{
	long int input;
	char *c, str[20];
	
	while(1)
	{
		print_string(msg);
		fgets(str, 20, stdin);
		input = strtol(str, &c, 10);
		if(*c == '\n' && input >= lbound && input <= rbound)
		{
			return input;
		}
		else if(*c != NULL)
		{
			printf("Wprowadzono nieprawid�owy format danych!\n");
		}
		else
		{
			print_string(warn);
		}
	}
}

static double dbl_get_user_input(const char *msg, const char *warn, const double lbound, const double rbound)
{
	double input;
	char *c, str[20];

	while(1)
	{
		print_string(msg);
		fgets(str, 20, stdin);
		input = strtod(str, &c);
		if(*c == '\n' && input >= lbound && input <= rbound)
		{
			return input;
		}
		else if(*c != NULL)
		{
			printf("Wprowadzono nieprawid�owy format danych!\n");
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
		switch(uint_get_user_input("Wybierz opcj�: ", "Nie ma takiej opcji!\n", 1, 7))
		{
		case 1:
			generate_signal();
			break;
		case 2:
			load_signal_from_file();
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
	printf("********MENU*******\n");
	printf("1. Wygeneruj sygna�\n");
	printf("2. Wczytaj sygna� z pliku\n");
	printf("3. Zapisz sygna� do pliku\n");
	printf("4. Oblicz DFT\n");
	printf("5. Zapisz DFT sygna�u do pliku\n");
	printf("6. Oblicz g��wn� sk�adow� cz�stotliwo�ciow�\n");
	printf("7. Zako�cz\n");
}

static void generate_signal_menu(const unsigned int num)
{
	printf("\nWybierz rodzaj sygna�u:\n");
	printf("1. Sinus\n");
	printf("2. Cosinus\n");
	printf("3. Prostok�t (symetryczny)\n");
	printf("4. Bia�y szum\n");
	printf("5. Generuj\n");
	if(num == 0 || num >= 5)
	{
		printf("Obecnie wybrano %d funkcji\n", num);
	}
	else if(num == 1)
	{
		printf("Obecnie wybrano %d funkcj�\n", num);
	}
	else
	{
		printf("Obecnie wybrano %d funkcje\n", num);
	}
}

static void generate_signal(void)
{
	double max_freq;
	unsigned int n_funcs, suggested_value;
	char n_samples_str[100], f_sampling_str[100];
	waveform_func *func;
	generator *tmp, *generators = NULL;

	n_funcs = max_freq = 0;
	while(1)
	{
		generate_signal_menu(n_funcs);
		switch(uint_get_user_input("Wybierz opcj�: ", "Nie ma takiej opcji\n", 1, 5))
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
			func = white_noise_generator;
			break;
		case 5:
			if(generators != NULL)
			{
				suggested_value = (int)ceil(2 * max_freq);
				
				sprintf(n_samples_str, "\nPodaj ilo�� pr�bek (sugerowana minimalna warto�� %d): ", suggested_value);
				n_samples = (unsigned int)uint_get_user_input(n_samples_str, "B��dna ilo�� pr�bek!\n", 1, LONG_MAX);
				
				sprintf(f_sampling_str, "Podaj cz�stotliwo�� pr�bkowania sygna�u[Hz] (sugerowana minimalna warto�� %d): ", suggested_value);
				f_sampling = dbl_get_user_input(f_sampling_str, "B��dna cz�stotliwo��!\n", 1.0/INT_MAX, INT_MAX);

				if(signal)
				{
					free(signal);
				}

				if(signal_generator(&signal, n_samples, f_sampling, n_funcs, generators))
				{
					printf("B��D przy alokacji pami�ci na pr�bki sygna�u. Sygna� nie zosta� wygenerowany!\n\n");
				}
				else
				{
					if(!fft)
					{
						printf("\nWYGENEROWANO SYGNA�!\n\n");
					}
					else
					{
						printf("\nWYGENEROWANO SYGNA�! DFT poprzedniego sygna�u zosta�o wykasowane\n\n");
						free(fft);
						fft = NULL;
					}
				}

				free(generators);
				generators = NULL;
			}
			else
			{
				printf("SYGNA� NIE ZOSTA� WYGENEROWANY. Nie zosta�y wybrane �adne funkcje!\n\n");
			}
			return;
		}

		//allocate new generator with waveform_func stored in func
		tmp = (generator *)realloc(generators, (n_funcs+1) * sizeof(generator));
		if(tmp == NULL)
		{
			printf("B��D przy alokacji pami�ci na now� funkcj�. Poprzednio wybrane funkcje nie zosta�y skaskowane!\n");
			continue;
		}

		generators = tmp;
		generators[n_funcs].func = func;
		generators[n_funcs].param.amp = dbl_get_user_input("\nPodaj amplitud�: ", "B��dna warto��!\n", 0, INT_MAX);
		if(func != white_noise_generator)
		{
			generators[n_funcs].param.freq = dbl_get_user_input("Podaj cz�stotliwo��[Hz]: ", "B��dna warto��!\n", 0, INT_MAX);
		}
		
		//store maximum frequency (needed to suggest f_sampling and n_samples value)
		if(generators[n_funcs].param.freq > max_freq) 
		{
			max_freq = generators[n_funcs].param.freq;
		}

		n_funcs++;
	}
}

static void save_signal_to_file(void)
{
	unsigned int i;
	char filename[100];
	FILE *pFile;

	if(signal != NULL)
	{
		printf("Podaj nazw� pliku: ");
		gets(filename);
		pFile = fopen(filename, "w");
		if(pFile != NULL)
		{
			fprintf(pFile, "%d;%lf\n", n_samples, f_sampling);
			for(i = 0; i < n_samples; ++i)
			{
				fprintf(pFile, "%lf\n", signal[i]);
			}
			fclose(pFile);

			printf("\nSYGNA� ZOSTA� POPRAWNIE ZAPISANY DO PLIKU: ");
			print_string(filename);
			printf("\n\n");
		}
		else
		{
			printf("\nB��D podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("\nW pami�ci nie ma sygna�u!\n\n");
	}
}

static void calculate_fft(void)
{
	if(!fft)
	{
		if(signal)
		{
			if(fft_to_frequency_domain(&signal, &fft, n_samples, f_sampling))
			{
				printf("\nB��D podczas obliczania DFT!\n\n");
			}
			else
			{
				printf("\nOBLICZONO DFT SYGNA�U!\n\n");
			}
		}
		else
		{
			printf("\nW pami�ci nie ma sygna�u!\n\n");
		}
	}
	else
	{
		printf("\nDFT dla bie��cego sygna�u ju� zosta�o obliczone!\n\n");
	}
}

static void save_fft_to_file(void)
{
	double freq;
	unsigned int i, length;
	char filename[100];
	FILE *pFile;

	if(fft)
	{
		printf("Podaj nazw� pliku: ");
		gets(filename);
		pFile = fopen(filename, "w");
		if(pFile != NULL)
		{
			length = next_pow_2(n_samples);
			fprintf(pFile, "Frequency[Hz];Re(DFT);Im(DFT)\n"); //header
			for(i = 0; i < length; ++i)
			{
				freq = i * ((double)f_sampling / length);
				fprintf(pFile, "%lf;%.30lf;%.30lf\n", freq, fft[i].Re, fft[i].Im);
			}
			fclose(pFile);
			printf("\nDFT ZOSTA�O POPRAWNIE ZAPISANE DO PLIKU: ");
			print_string(filename);
			printf("\n\n");
		}
		else
		{
			printf("\nB��D podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("\nDFT dla bie��cego sygna�u nie zosta�o jeszcze obliczone!\n\n");
	}
}

static void get_main_frequency(void)
{
	double main_freq;

	if(fft)
	{
		main_freq = fft_get_main_frequency(fft, next_pow_2(n_samples), f_sampling);
		printf("\nG��WNA SK�ADOWA TO: %lf\n\n", main_freq);
	}
	else
	{
		printf("\nDFT dla bie��cego sygna�u nie zosta�o jeszcze obliczone!\n\n");
	}
}

static void load_signal_from_file(void)
{
	double tmp_value, tmp_f_sampling, *tmp_signal = NULL;
	unsigned int tmp_n_samples, i;
	char filename[100];
	FILE *pFile;

	printf("Podaj nazw� pliku do wczytania: ");
	gets(filename);

	pFile = fopen(filename, "r");
	if(pFile != NULL)
	{
		switch(fscanf(pFile, "%d;%lf\n", &tmp_n_samples, &tmp_f_sampling))
		{
		case 2:
			break;
		default:
			fclose(pFile);
			printf("\nB��D podczas odczytu pliku!\n\n");
			return;
		}

		tmp_signal = (double *)malloc(tmp_n_samples * sizeof(double));
		if(tmp_signal == NULL)
		{
			printf("\nB��D podczas alokacji pami�ci na sygna�!\n\n");
			return;
		}

		i = 0;
		while(fscanf(pFile, "%lf\n", &tmp_value) != EOF)
		{
			tmp_signal[i++] = tmp_value;
		}
		fclose(pFile);
	}
	else
	{
		printf("\nB��D podczas otwierania pliku!\n\n");
		return;
	}

	if(i != tmp_n_samples)
	{
		printf("\nB��D sygna� nie zosta� wczytany poprawnie. Brak wszystkich danych.\n\n");
		return;
	}

	printf("\nWCZYTANO POPRAWNIE SYGNA� Z PLIKU: ");
	print_string(filename);
	
	if(fft)
	{
		free(fft);
		fft = NULL;
		printf("\nDFT poprzedniego sygna�u zosta�o wykasowane!");
	}

	printf("\n\n");
	n_samples = tmp_n_samples;
	f_sampling = tmp_f_sampling;

	if(signal)
	{
		free(signal);
	}
	signal = tmp_signal;
}

static void end(void)
{
	if(signal)
	{
		free(signal);
	}

	if(fft)
	{
		free(fft);
	}
}