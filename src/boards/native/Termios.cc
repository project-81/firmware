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

    stream << "input  baud: " << speed_str(cfgetispeed(&current)) << std::endl;
    stream << "output baud: " << speed_str(cfgetospeed(&current)) << std::endl;
}

bool Termios::make_raw(int optional_actions)
{
    cfmakeraw(&current);
    return setattrs(optional_actions);
}

bool Termios::set_echo(bool state, int optional_actions)
{
    current.c_lflag =
        (state) ? current.c_lflag | ECHO : current.c_lflag & ~(ECHO);
    return setattrs(optional_actions);
}

bool Termios::set_canonical(bool state, int optional_actions)
{
    current.c_lflag =
        (state) ? current.c_lflag | ICANON : current.c_lflag & ~(ICANON);
    return setattrs(optional_actions);
}

bool Termios::setattrs(int optional_actions)
{
    if (valid)
    {
        valid = tcsetattr(fd, optional_actions, &current) == 0;
        print_verb_name_condition("tc", "setattr", valid,
                                  true /* show_errno */,
                                  true /* error_only */);
    }

    return valid;
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
    valid = tcsetattr(fd, default_action, &original) == 0;
    print_verb_name_condition("tc", "setattr", valid, true /* show_errno */,
                              true /* error_only */);
}

} // namespace Project81
