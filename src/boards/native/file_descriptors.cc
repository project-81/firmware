/* toolchain */
#include <cstring>
#include <iostream>
#include <unistd.h>

/* internal */
#include "boards/native/file_descriptors.h"
#include "boards/native/text.h"

namespace Project81
{

void close_fds(const FdMap &fds)
{
    for (const auto &[name, fd] : fds)
    {
        print_verb_name_condition(name, "close", close(fd) == 0);
    }
}

bool get_rw_file_fd(const std::string path, FdMap &fds)
{
    FILE *file = std::fopen(path.data(), "r+");
    bool result = file != nullptr;

    if (result)
    {
        int fd = fileno(std::fopen(path.data(), "r+"));
        result = fd > 0;
        if (result)
        {
            fds[path] = fd;
        }
    }

    print_verb_name_condition(path, "open", result);
    return result;
}

} // namespace Project81
