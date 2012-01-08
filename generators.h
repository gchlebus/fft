#ifndef GENERATORS_H
#define GENERATORS_H

typedef struct sig_param_ 
{
	double amp;
	double freq;
} sig_param;

extern struct _complex;
typedef struct _complex complex;

typedef void waveform_func(complex *input, const unsigned int length, const double f_sampling, const sig_param param);

typedef struct generator_
{
	waveform_func *func;
	sig_param param;
} generator;

/**
 *Generates sine waveform. Generated samples are added to values in |input|.
 *
 *@param input - pointer to array where generated values will be stored.
 *@param length - length of signal i.e. number of samples to be generated.
 *@param f_sampling - signal sampling frequency.
 *@param param - signal parameters i.e. amplitude and frequency.
 *
**/
void sine_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param);

/**
 *Generates cosine waveform. Generated samples are added to values in |input|.
 *
 *@param input - pointer to array where generated values will be stored.
 *@param length - length of signal i.e. number of samples to be generated.
 *@param f_sampling - signal sampling frequency.
 *@param param - signal parameters i.e. amplitude and frequency.
 *
**/
void cosine_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param);

/**
 *Generates rectangle waveform. Generated samples are added to values in |input|.
 *
 *@param input - pointer to array where generated values will be stored.
 *@param length - length of signal i.e. number of samples to be generated.
 *@param f_sampling - signal sampling frequency.
 *@param param - signal parameters i.e. amplitude and frequency.
 *
**/
void rectangle_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param);

/**
 *Generates white noise. Generated samples are added to values in |input|.
 *
 *@param input - pointer to array where generated values will be stored.
 *@param length - length of signal i.e. number of samples to be generated.
 *@param f_sampling - signal sampling frequency.
 *@param param - signal parameters i.e. amplitude and frequency (not used in this case).
 *
**/
void white_noise_generator(complex *input, const unsigned int length, const double f_sampling, const sig_param param);

/**
 *Generates signal compunding of waveform functions stored in |generators|.
 *
 *@param signal - array where signal will be stored.
 *@param length - length of signal i.e. number of samples to be generated.
 *@param f_sampling - signal sampling frequency.
 *@param n - number of waveform functions stored in |generators|.
 *@param generators - array of generator objects.
 *
 *@return 0 if successful, otherwise -1.
 *
**/
int signal_generator (complex **signal, const unsigned int length, const double f_sampling, const unsigned int n, const generator *generators);

#endif
