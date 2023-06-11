/* toolchain */
#include <cassert>
#include <cstdio>

/* internal */
#include "boards/native/Termios.h"
#include "boards/native/termios_util.h"
#include "boards/native/text.h"

namespace Project81
{

void Termios::info(std::ostream &stream)
{
    dump_input_modes(stream, current);
    dump_output_modes(stream, current);
    dump_control_modes(stream, current);
    dump_local_modes(stream, current);
    dump_specials(stream, current);
}

Termios::Termios(int _fd) : fd(_fd), valid(true)
{
    /*
     * Store the original termios structure before it's modified.
     */
    valid = tcgetattr(fd, &original) == 0;
    print_verb_name_condition("tc", "getattr", valid, true /* show_errno */,
                              true /* error_only */);
    current = original;
}

Termios::~Termios()
{
    /*
     * Restore the original termios structure.
     */
    valid = tcsetattr(fd, TCSAFLUSH, &original) == 0;
    print_verb_name_condition("tc", "setattr", valid, true /* show_errno */,
                              true /* error_only */);
}

} // namespace Project81
