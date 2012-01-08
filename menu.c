#define _COMPLEX_DEFINED
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "menu.h"
#include "generators.h"
#include "FFT.h"

complex *signal;
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
		fflush(stdin);
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

static double dbl_get_user_input(const char *msg, const char *warn, const double lbound, const double rbound)
{
	double input;
	char *c, str[20];

	while(1)
	{
		print_string(msg);
		fflush(stdin);
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
		switch(uint_get_user_input("Wybierz opcjê: ", "Nie ma takiej opcji!\n", 1, 8))
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
			filter();
			break;
		case 6:
			save_fft_to_file();
			break;
		case 7:
			get_main_frequency();
			break;
		case 8:
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
	printf("5. Filtruj sygna³\n");
	printf("6. Zapisz DFT sygna³u do pliku\n");
	printf("7. Oblicz g³ówn¹ sk³adow¹ czêstotliwoœciow¹\n");
	printf("8. Zakoñcz\n");
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
	double max_freq;
	unsigned int n_funcs, suggested_value;
	char n_samples_str[100], f_sampling_str[100];
	waveform_func *func;
	generator *tmp, *generators = NULL;

	n_funcs = max_freq = 0;
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
			srand(time(NULL));
			break;
		case 5:
			if(generators != NULL)
			{
				suggested_value = (int)ceil(2 * max_freq);
				
				sprintf(f_sampling_str, "Podaj czêstotliwoœæ próbkowania sygna³u[Hz] (sugerowana minimalna wartoœæ %d): ", suggested_value);
				f_sampling = dbl_get_user_input(f_sampling_str, "B³êdna czêstotliwoœæ!\n", 1.0/INT_MAX, INT_MAX);

				sprintf(n_samples_str, "\nPodaj iloœæ próbek (sugerowana minimalna wartoœæ %d): ", next_pow_2((int)(f_sampling / max_freq)));
				n_samples = (unsigned int)uint_get_user_input(n_samples_str, "B³êdna iloœæ próbek!\n", 1, LONG_MAX);
				
				if(signal)
				{
					free(signal);
					//signal = NULL;
				}

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
				fprintf(pFile, "%lf;%lf\n", signal[i].Re, signal[i].Im);
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
	if(!fft)
	{
		if(signal)
		{
			if(fft_to_frequency_domain(&signal, &fft, n_samples))
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
	else
	{
		printf("\nDFT dla bie¿¹cego sygna³u ju¿ zosta³o obliczone!\n\n");
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
		printf("Podaj nazwê pliku: ");
		gets(filename);
		pFile = fopen(filename, "w");
		if(pFile != NULL)
		{
			length = next_pow_2(n_samples);
			fprintf(pFile, "Frequency[Hz];|DFT|\n");
			for(i = 0; i < length; ++i)
			{
				freq = i * ((double)f_sampling / length);
				fprintf(pFile, "%lf;%.30lf\n", freq, sqrt(pow(fft[i].Re, 2) + pow(fft[i].Im, 2)) / next_pow_2(n_samples));
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

	if(fft)
	{
		main_freq = fft_get_main_frequency(fft, next_pow_2(n_samples), f_sampling);
		printf("\nG£ÓWNA SK£ADOWA TO: %lf\n\n", main_freq);
	}
	else
	{
		printf("\nDFT dla bie¿¹cego sygna³u nie zosta³o jeszcze obliczone!\n\n");
	}
}

static void load_signal_from_file(void)
{
	double tmp_value_Re, tmp_value_Im, tmp_f_sampling;
	unsigned int tmp_n_samples, i;
	unsigned char count;
	char filename[100];
	complex *tmp_signal = NULL;
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

		tmp_signal = (complex *)calloc(tmp_n_samples, sizeof(complex));
		if(tmp_signal == NULL)
		{
			printf("\nB£¥D podczas alokacji pamiêci na sygna³!\n\n");
			return;
		}

		i = 0;
		while((count = fscanf(pFile, "%lf;%lf\n", &tmp_value_Re, &tmp_value_Im)) != EOF)
		{
			tmp_signal[i].Re = tmp_value_Re;
			if(count == 2)
			{
				tmp_signal[i].Im = tmp_value_Im;
			}
			i++;
		}
		fclose(pFile);
	}
	else
	{
		printf("\nB£¥D podczas otwierania pliku!\n\n");
		return;
	}

	if(i != tmp_n_samples)
	{
		printf("\nB£¥D sygna³ nie zosta³ wczytany poprawnie. Brak wszystkich danych.\n\n");
		return;
	}

	printf("\nWCZYTANO POPRAWNIE SYGNA£ Z PLIKU: ");
	print_string(filename);
	
	if(fft)
	{
		free(fft);
		fft = NULL;
		printf("\nDFT poprzedniego sygna³u zosta³o wykasowane!");
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

void filter(void)
{
	double cutoff_freq_l, cutoff_freq_u, freq;
	unsigned int next_pow_2_num, i;
	
	if(!fft)
	{
		printf("\nDFT dla bie¿¹cego sygna³u nie zosta³o jeszcze obliczone!\n\n");
		return;
	}

	next_pow_2_num = next_pow_2(n_samples); //number of elements in dft array

	printf("1. Dolnoprzepustowy\n");
	printf("2. Górnoprzepustowy\n");
	printf("3. Pasmowoprzepustowy\n");
	
	switch(uint_get_user_input("Wybierz rodzaj filtra: ", "Z³y format danych!", 1, 3))
	{
	case 1:
		cutoff_freq_l = dbl_get_user_input("FILTR DOLNOPRZEPUSTOWY\nPodaj czêstotliwoœæ graniczn¹: ", "Z³y format danych!", 0, INT_MAX);

		for(i = 0; i < next_pow_2_num / 2; ++i)
		{
			freq = i * ((double)f_sampling / next_pow_2_num);

			if( !(freq < cutoff_freq_l) )
			{
				fft[i].Re = fft[i].Im = 0;
				fft[next_pow_2_num - (i + 1)].Re = fft[next_pow_2_num - (i + 1)].Im = 0;
			}
		}
		break;

	case 2:
		cutoff_freq_u = dbl_get_user_input("FILTR GÓRNOPRZEPUSTOWY\nPodaj czêstotliwoœæ graniczn¹: ", "Z³y format danych!", 0, INT_MAX);

		for(i = 0; i < next_pow_2_num / 2; ++i)
		{
			freq = i * ((double)f_sampling / next_pow_2_num);

			if( !(freq > cutoff_freq_u) )
			{
				fft[i].Re = fft[i].Im = 0;
				fft[next_pow_2_num - (i + 1)].Re = fft[next_pow_2_num - (i + 1)].Im = 0;
			}
		}
		break;

	case 3:
		cutoff_freq_l = dbl_get_user_input("FILTR PASMOWOPRZEPUSTOWY\nPodaj doln¹ czêstotliwoœæ graniczn¹: ", "Z³y format danych!", 0, INT_MAX);
		cutoff_freq_u = dbl_get_user_input("FILTR PASMOWOPRZEPUSTOWY\nPodaj górn¹ czêstotliwoœæ graniczn¹: ", "Z³y format danych!", 0, INT_MAX);

		for(i = 0; i < next_pow_2_num / 2; ++i)
		{
			freq = i * ((double)f_sampling / next_pow_2_num);

			if( !(freq < cutoff_freq_u) || !(freq > cutoff_freq_l) )
			{
				fft[i].Re = fft[i].Im = 0;
				fft[next_pow_2_num - (i + 1)].Re = fft[next_pow_2_num - (i + 1)].Im = 0;
			}
		}
		break;
	}
	
	if(fft_to_time_domain(fft, &signal, next_pow_2_num))
	{
		printf("\nOperacja filtracji nie powiod³a siê!\n\n");
		return;
	}

	printf("\nOperacja filtracji siê powiod³a!\nDFT zosta³o zmodyfikowane!\nSygna³ zosta³ nadpisany!\n\n");
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