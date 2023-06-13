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

const char *speed_str(speed_t data)
{
    switch (data)
    {
    case B0:
        return "0";
    case B50:
        return "50";
    case B75:
        return "75";
    case B110:
        return "110";
    case B134:
        return "134";
    case B150:
        return "150";
    case B200:
        return "200";
    case B300:
        return "300";
    case B600:
        return "600";
    case B1200:
        return "1200";
    case B1800:
        return "1800";
    case B2400:
        return "2400";
    case B4800:
        return "4800";
    case B9600:
        return "9600";
    case B19200:
        return "19200";
    case B38400:
        return "38400";
    case B57600:
        return "57600";
    case B115200:
        return "115200";
    case B230400:
        return "230400";
    case B460800:
        return "460800";
    case B500000:
        return "500000";
    case B576000:
        return "576000";
    case B921600:
        return "921600";
    case B1000000:
        return "1000000";
    case B1152000:
        return "1152000";
    case B1500000:
        return "1500000";
    case B2000000:
        return "2000000";
    case B2500000:
        return "2500000";
    case B3000000:
        return "3000000";
    case B3500000:
        return "3500000";
    case B4000000:
        return "4000000";
    default:
        return "UNKNOWN";
    }
}

} // namespace Project81
