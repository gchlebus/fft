#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "menu.h"

int main(void)
{
	setlocale(LC_ALL, "polish");
	
	menu();

	return 0;
}
