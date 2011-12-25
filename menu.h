#ifndef MENU_H
#define MENU_H

void menu(void);

static void print_string(const char* str);

static long int uint_get_user_input(const char *msg, const char *warn, const unsigned int lbound, const unsigned int rbound);

static double dbl_get_user_input(const char* msg, const char *warn, const double lbound, const double rbound);

static void main_menu(void);

static void load_signal_from_file(void);

static void generate_signal_menu(const unsigned int num);

static void generate_signal(void);

static void save_signal_to_file(void);

static void calculate_fft(void);

static void save_fft_to_file(void);

static void get_main_frequency(void);

static void end(void);

#endif