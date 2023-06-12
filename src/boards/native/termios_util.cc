/* toolchain */
#include <cassert>
#include <map>
#include <vector>

/* internal */
#include "boards/native/termios_util.h"
#include "boards/native/text.h"

namespace Project81
{

static const std::map<std::string, tcflag_t> input_flags = {
    {"IGNBRK", IGNBRK}, {"BRKINT", BRKINT},   {"IGNPAR", IGNPAR},
    {"PARMRK", PARMRK}, {"INPCK", INPCK},     {"ISTRIP", ISTRIP},
    {"INLCR", INLCR},   {"IGNCR", IGNCR},     {"ICRNL", ICRNL},
    {"IUCLC", IUCLC},   {"IXON", IXON},       {"IXANY", IXANY},
    {"IXOFF", IXOFF},   {"IMAXBEL", IMAXBEL}, {"IUTF8", IUTF8},
};

void dump_input_modes(std::ostream &stream, const struct termios &data)
{
    stream << "  input modes (c_iflag): ";

    /* Collect flags. */
    std::vector<std::string> flag_strs;
    populate_flag_strs<tcflag_t>(data.c_iflag, flag_strs, input_flags,
                                 &stream);
}

static const std::map<std::string, tcflag_t> output_flags = {
    {"OPOST", OPOST}, {"OLCUC", OLCUC},   {"ONLCR", ONLCR},   {"OCRNL", OCRNL},
    {"ONOCR", ONOCR}, {"ONLRET", ONLRET}, {"OFILL", OFILL},   {"OFDEL", OFDEL},
    {"NLDLY", NLDLY}, {"CRDLY", CRDLY},   {"TABDLY", TABDLY}, {"BSDLY", BSDLY},
    {"VTDLY", VTDLY}, {"FFDLY", FFDLY},
};

void dump_output_modes(std::ostream &stream, const struct termios &data)
{
    stream << " output modes (c_oflag): ";

    /* Collect flags. */
    std::vector<std::string> flag_strs;
    populate_flag_strs<tcflag_t>(data.c_oflag, flag_strs, output_flags,
                                 &stream);
}

static const std::map<std::string, tcflag_t> control_flags = {
    {"CBAUD", CBAUD},
    {"CBAUDEX", CBAUDEX},
    {"CSIZE", CSIZE},
    {"CSTOPB", CSTOPB},
    {"CREAD", CREAD},
    {"PARENB", PARENB},
    {"PARODD", PARODD},
    {"HUPCL", HUPCL},
    {"CLOCAL", CLOCAL},
    //{"LOBLK", LOBLK},
    {"CIBAUD", CIBAUD},
    {"CMSPAR", CMSPAR},
    {"CRTSCTS", CRTSCTS},
};

void dump_control_modes(std::ostream &stream, const struct termios &data)
{
    stream << "control modes (c_cflag): ";

    /* Collect flags. */
    std::vector<std::string> flag_strs;
    populate_flag_strs<tcflag_t>(data.c_cflag, flag_strs, control_flags,
                                 &stream);
}

static const std::map<std::string, tcflag_t> local_flags = {
    {"ISIG", ISIG},
    {"ICANON", ICANON},
    {"XCASE", XCASE},
    {"ECHO", ECHO},
    {"ECHOE", ECHOE},
    {"ECHOK", ECHOK},
    {"ECHONL", ECHONL},
    {"ECHOCTL", ECHOCTL},
    {"ECHOPRT", ECHOPRT},
    {"ECHOKE", ECHOKE},
    //{"DEFECHO", DEFECHO},
    {"FLUSHO", FLUSHO},
    {"NOFLSH", NOFLSH},
    {"TOSTOP", TOSTOP},
    {"PENDIN", PENDIN},
    {"IEXTEN", IEXTEN},
};

void dump_local_modes(std::ostream &stream, const struct termios &data)
{
    stream << "  local modes (c_lflag): ";

    /* Collect flags. */
    std::vector<std::string> flag_strs;
    populate_flag_strs<tcflag_t>(data.c_lflag, flag_strs, local_flags,
                                 &stream);
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
