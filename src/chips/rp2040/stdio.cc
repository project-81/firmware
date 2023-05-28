/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/UartManager.h"
#include "common/buffer/PcBuffer.h"

static constexpr std::size_t uart_depth = 32;

using UartManager = Project81::UartManager<uart_depth, uart_depth>;

static UartManager manager = UartManager(uart0);

static int putc_wrapper(char c, FILE *file)
{
    (void)file;
    manager.putc_block(c);
    return 0;
}

static int flush_wrapper(FILE *file)
{
    (void)file;
    manager.flush();
    return 0;
}

static FILE __stdio =
    FDEV_SETUP_STREAM(putc_wrapper, NULL, flush_wrapper, _FDEV_SETUP_WRITE);

FILE *const stdout = &__stdio;
FILE *const stdin = nullptr;
FILE *const stderr = nullptr;

bool Project81::getc_nonblocking(uint8_t &data)
{
    return manager.getc_nonblocking(data);
}
