#pragma once

#include <stdbool.h>

namespace Project81
{
__attribute__((noreturn)) void reset(bool bootloader);
void dump_clocks();
}; // namespace Project81
