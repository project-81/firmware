/* toolchain */
#include <stdio.h>

/* third-party */
#include "hardware/uart.h"

static int putc_wrapper(char c, FILE *file)
{
    (void)file;

    while (!uart_is_writable(uart0))
    {
        tight_loop_contents();
    }
    uart_get_hw(uart0)->dr = c;

    return 0;
}

static FILE __stdio =
    FDEV_SETUP_STREAM(putc_wrapper, NULL, NULL, _FDEV_SETUP_WRITE);

FILE *const stdout = &__stdio;
