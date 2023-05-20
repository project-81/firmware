#include "pico/stdlib.h"
#include <stdio.h>

static int putc_wrapper(char c, FILE *file)
{
    (void)file;
    return putchar_raw(c);
}

static int flush_wrapper(FILE *file)
{
    (void)file;
    stdio_flush();
    return 0;
}

static FILE __stdio =
    FDEV_SETUP_STREAM(putc_wrapper, NULL, flush_wrapper, _FDEV_SETUP_WRITE);

FILE *const stdout = &__stdio;
