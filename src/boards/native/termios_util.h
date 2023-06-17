#pragma once

/* linux */
#include <termios.h>

/* toolchain */
#include <iostream>

namespace Project81
{

void dump_input_modes(std::ostream &stream, const struct termios &data);
void dump_output_modes(std::ostream &stream, const struct termios &data);
void dump_control_modes(std::ostream &stream, const struct termios &data);
void dump_local_modes(std::ostream &stream, const struct termios &data);
void dump_specials(std::ostream &stream, const struct termios &data);

const char *speed_str(speed_t data);

} // namespace Project81
