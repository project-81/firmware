/* toolchain */
#include <cstring>
#include <iostream>

/* internal */
#include "boards/native/text.h"

namespace Project81
{

std::ostream &get_stdout(bool status)
{
    return (status) ? std::cout : std::cerr;
}

void print_verb_name_condition(const std::string name, const std::string verb,
                               bool status, bool show_errno, bool error_only)
{
    if (not error_only or not status)
    {
        std::ostream &stream = get_stdout(status);
        stream << ((status) ? "Could " : "Couldn't ") << verb << " '" << name
               << "'";

        if (!status and show_errno)
        {
            stream << " (" << std::strerror(errno) << ")." << std::endl;
        }

        stream << "." << std::endl;
    }
}

}; // namespace Project81
