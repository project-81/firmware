/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/UartManager.h"
#include "common/buffer/PcBuffer.h"

using UartManager = Project81::UartManager<32, 32>;

static UartManager manager = UartManager(uart0);

static int putc_wrapper(char c, FILE *file)
{
    (void)file;
    manager.putc_block_if_full(c);
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

void Project81::service_stdio_uart(void)
{
    manager.dispatch();
}

bool Project81::getc_nonblocking(std::uint8_t &data)
{
    return manager.getc_nonblocking(data);
}
