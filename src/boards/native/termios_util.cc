/* toolchain */
#include <cassert>

/* internal */
#include "boards/native/termios_util.h"

namespace Project81
{

void dump_input_modes(std::ostream &stream, const struct termios &data)
{
    stream << "  input modes (c_iflag): 0x" << std::hex << data.c_iflag
           << std::endl;
}

void dump_output_modes(std::ostream &stream, const struct termios &data)
{
    stream << " output modes (c_oflag): 0x" << std::hex << data.c_oflag
           << std::endl;
}

void dump_control_modes(std::ostream &stream, const struct termios &data)
{
    stream << "control modes (c_cflag): 0x" << std::hex << data.c_cflag
           << std::endl;
}

void dump_local_modes(std::ostream &stream, const struct termios &data)
{
    stream << "  local modes (c_lflag): 0x" << std::hex << data.c_lflag
           << std::endl;
}

void dump_specials(std::ostream &stream, const struct termios &data)
{
    stream << "special                : ";
    for (int i = 0; i < NCCS; i++)
    {
        int curr = static_cast<int>(data.c_cc[i]);

        std::string prefix = (i != 0) ? ", " : "";
        prefix += "(" + std::to_string(i) + ") ";

        switch (i)
        {
        case VDISCARD:
            stream << prefix << "VDISCARD";
            break;
        case VEOF:
            stream << prefix << "VEOF";
            break;
        case VEOL:
            stream << prefix << "VEOL";
            break;
        case VEOL2:
            stream << prefix << "VEOL2";
            break;
        case VERASE:
            stream << prefix << "VERASE";
            break;
        case VINTR:
            stream << prefix << "VINTR";
            break;
        case VKILL:
            stream << prefix << "VKILL";
            break;
        case VLNEXT:
            stream << prefix << "VLNEXT";
            break;
        case VMIN:
            stream << prefix << "VMIN";
            break;
        case VQUIT:
            stream << prefix << "VQUIT";
            break;
        case VREPRINT:
            stream << prefix << "VREPRINT";
            break;
        case VSTART:
            stream << prefix << "VSTART";
            break;
        case VSTOP:
            stream << prefix << "VSTOP";
            break;
        case VSUSP:
            stream << prefix << "VSUSP";
            break;
        case VTIME:
            stream << prefix << "VTIME";
            break;
        case VWERASE:
            stream << prefix << "VWERASE";
            break;
        /* VDSUSP not defined? */
        /* VSTATUS not defined? */
        /* VSWITCH not defined? */
        default:
            /*
             * If the index doesn't have a defined role, it shouldn't have a
             * character mapping.
             */
            assert(curr == 0);
            continue;
        }

        if (curr != 0)
        {
            stream << " 0x" << std::hex << curr;
        }
        else
        {
            stream << " disabled";
        }
    }

    stream << std::endl;
}

} // namespace Project81
