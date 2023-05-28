#pragma once

/* toolchain */
#include <stdbool.h>
#include <stdint.h>

namespace Project81
{

__attribute__((noreturn)) void reset(bool bootloader);
void dump_clocks(void);

/*
 * Since a UART inevitably plays a role in standard input/output,
 * forward-declare some functions for implementing the desired behavior.
 */
bool getc_nonblocking(uint8_t &data);

}; // namespace Project81
