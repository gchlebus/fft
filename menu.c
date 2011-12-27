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

static long int uint_get_user_input(const char *msg,  const char * warn, const unsigned int lbound, const unsigned int rbound)
{
	char *c, str[20];
	unsigned int input;
	
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
			printf("Wprowadzono nieprawid³owy format danych!\n");
		}
		else
		{
			print_string(warn);
		}
	}
}

static double dbl_get_user_input(const char* msg, const char *warn, const double lbound, const double rbound)
{
	char *c, str[20];
	double input;

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
	printf("1. Wygeneruj sygna³\n");
	printf("2. Wczytaj sygna³ z pliku\n");
	printf("3. Zapisz sygna³ do pliku\n");
	printf("4. Oblicz DFT\n");
	printf("5. Zapisz DFT sygna³u do pliku\n");
	printf("6. Oblicz g³ówn¹ sk³adow¹ czêstotliwoœciow¹\n");
	printf("7. Zakoñcz\n");
}

static void generate_signal_menu(const unsigned int num)
{
	printf("\nWybierz rodzaj sygna³u:\n");
	printf("1. Sinus\n");
	printf("2. Cosinus\n");
	printf("3. Prostok¹t (symetryczny)\n");
	printf("4. Bia³y szum\n");
	printf("5. Generuj\n");
	if(num == 0 || num >= 5)
	{
		printf("Obecnie wybrano %d funkcji\n", num);
	}
	else if(num == 1)
	{
		printf("Obecnie wybrano %d funkcjê\n", num);
	}
	else
	{
		printf("Obecnie wybrano %d funkcje\n", num);
	}
}

static void generate_signal(void)
{
	unsigned int n_funcs, suggested_value;
	double max_freq = 0;
	char n_samples_str[100], f_sampling_str[100];
	waveform_func *func;
	generator *tmp, *generators = NULL;

	n_funcs = 0;
	while(1)
	{
		generate_signal_menu(n_funcs);
		switch(uint_get_user_input("Wybierz opcjê: ", "Nie ma takiej opcji\n", 1, 5))
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
				
				sprintf(n_samples_str, "\nPodaj iloœæ próbek (sugerowana minimalna wartoœæ %d): ", suggested_value);
				n_samples = uint_get_user_input(n_samples_str, "B³êdna iloœæ próbek!\n", 1, UINT_MAX);
				
				sprintf(f_sampling_str, "Podaj czêstotliwoœæ próbkowania sygna³u[Hz] (sugerowana minimalna wartoœæ %d): ", suggested_value);
				f_sampling = dbl_get_user_input(f_sampling_str, "B³êdna czêstotliwoœæ!\n", 1.0/INT_MAX, INT_MAX);

				if(signal_generator(&signal, n_samples, f_sampling, n_funcs, generators))
				{
					printf("B£¥D przy alokacji pamiêci na próbki sygna³u. Sygna³ nie zosta³ wygenerowany!\n\n");
				}
				else
				{
					if(!fft)
					{
						printf("\nWYGENEROWANO SYGNA£!\n\n");
					}
					else
					{
						printf("\nWYGENEROWANO SYGNA£! DFT poprzedniego sygna³u zosta³o wykasowane\n\n");
						free(fft);
						fft = NULL;
					}
				}

				free(generators);
				generators = NULL;
			}
			else
			{
				printf("SYGNA£ NIE ZOSTA£ WYGENEROWANY. Nie zosta³y wybrane ¿adne funkcje!\n\n");
			}
			return;
		}

		//allocate new generator with waveform_func stored in func
		tmp = (generator *)realloc(generators, (n_funcs+1) * sizeof(generator));
		if(tmp == NULL)
		{
			printf("B£¥D przy alokacji pamiêci na now¹ funkcjê. Poprzednio wybrane funkcje nie zosta³y skaskowane!\n");
			continue;
		}

		generators = tmp;
		generators[n_funcs].func = func;
		generators[n_funcs].param.amp = dbl_get_user_input("\nPodaj amplitudê: ", "B³êdna wartoœæ!\n", 0, INT_MAX);
		if(func != white_noise_generator)
		{
			generators[n_funcs].param.freq = dbl_get_user_input("Podaj czêstotliwoœæ[Hz]: ", "B³êdna wartoœæ!\n", 0, INT_MAX);
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
		printf("Podaj nazwê pliku: ");
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

			printf("\nSYGNA£ ZOSTA£ POPRAWNIE ZAPISANY DO PLIKU: ");
			print_string(filename);
			printf("\n\n");
		}
		else
		{
			printf("\nB£¥D podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("\nW pamiêci nie ma sygna³u!\n\n");
	}
}

static void calculate_fft(void)
{
	if(signal != NULL)
	{
		if(fft_to_frequency_domain(&signal, &fft, n_samples, f_sampling))
		{
			printf("\nB£¥D podczas obliczania DFT!\n\n");
		}
		else
		{
			printf("\nOBLICZONO DFT SYGNA£U!\n\n");
		}
	}
	else
	{
		printf("\nW pamiêci nie ma sygna³u!\n\n");
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
			for(i = 0; i < length; ++i)
			{
				freq = i * ((double)f_sampling / length);
				fprintf(pFile, "%lf;%.30lf\n", freq, (sqrt(pow(fft[i].Re,2) + pow(fft[i].Im,2))) / length);
			}
			fclose(pFile);
			printf("\nDFT ZOSTA£O POPRAWNIE ZAPISANE DO PLIKU: ");
			print_string(filename);
			printf("\n\n");
		}
		else
		{
			printf("\nB£¥D podczas zapisu do pliku!\n\n");
		}
	}
	else
	{
		printf("\nDFT dla bie¿¹cego sygna³u nie zosta³o jeszcze obliczone!\n\n");
	}
}

static void get_main_frequency(void)
{
	double main_freq;

	if(fft != NULL)
	{
		main_freq = fft_get_main_frequency(fft, next_pow_2(n_samples), f_sampling);
		printf("\nG£ÓWNA SK£ADOWA TO: %.4lf\n\n", main_freq);
	}
	else
	{
		printf("\nDFT dla bie¿¹cego sygna³u nie zosta³o jeszcze obliczone!\n\n");
	}
}

static void load_signal_from_file(void)
{
	double *tmp, *tmp_signal = NULL;
	double tmp_f_sampling;
	unsigned int tmp_n_samples;
	double value;
	char filename[100];
	unsigned int max_size, size;
	FILE *pFile;

	printf("Podaj nazwê pliku do wczytania: ");
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
			printf("\nB£¥D podczas odczytu pliku!\n\n");
			return;
		}

		max_size = size = 0;
		while(1)
		{
			if(fscanf(pFile, "%lf\n", &value) == 1)
			{
				if(size >= max_size)
				{
					max_size = 2 * size + 2;
					tmp = realloc(tmp_signal, max_size * sizeof(double));
					if(tmp == NULL)
					{
						printf("\nB£¥D podczas alokacji pamiêci! Sygna³ nie zosta³ wczytany.\n\n");
						free(tmp_signal);
						return;
					}

					tmp_signal = tmp;
				}

				*(tmp_signal + size++) = value;
			}
			else
			{
				fclose(pFile);
				break;
			}
		}
	}
	else
	{
		printf("\nB£¥D podczas otwierania pliku!\n\n");
		return;
	}

	if(size != tmp_n_samples)
	{
		printf("\nB£¥D sygna³ nie zosta³ wczytany poprawnie. Brak wszystkich danych.\n\n");
		return;
	}

	printf("\nWCZYTANO POPRAWNIE SYGNA£ Z PLIKU: ");
	print_string(filename);
	printf("\n\n");
	n_samples = tmp_n_samples;
	f_sampling = tmp_f_sampling;
	signal = tmp_signal;
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