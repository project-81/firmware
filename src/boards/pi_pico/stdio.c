#include "pico/stdlib.h"
#include <stdio.h>

static int
sample_putc(char c, FILE *file)
{
	(void) file;
	return putchar_raw(c);
}

static FILE __stdio = FDEV_SETUP_STREAM(sample_putc,
					NULL,
					NULL,
					_FDEV_SETUP_WRITE);

FILE *const stdout = &__stdio;
