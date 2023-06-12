/* linux */
#include <fcntl.h>

/* toolchain */
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>

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

bool get_file_fd(const std::string path, FdMap &fds, const std::string mode)
{
    bool result = not fds.contains(path);

    if (result)
    {
        FILE *file = std::fopen(path.data(), mode.data());
        result = file != nullptr;

        if (result)
        {
            int fd = fileno(std::fopen(path.data(), mode.data()));
            result = fd != -1;
            if (result)
            {
                fds[path] = fd;
            }
        }
    }

    print_verb_name_condition(path, "open", result);
    return result;
}

static const std::map<std::string, int> status_flags = {
    /* Access modes. */
    {"O_RDONLY", O_RDONLY},
    {"O_WRONLY", O_WRONLY},
    {"O_RDWR", O_RDWR},
    /* Open-time flags. */
    {"O_CREAT", O_CREAT},
    {"O_EXCL", O_EXCL},
    {"O_NOCTTY", O_NOCTTY},
    {"O_TRUNC", O_TRUNC},
    {"O_APPEND", O_APPEND},
    {"O_ASYNC", O_ASYNC},
    {"O_DIRECT", O_DIRECT},
    {"O_NOATIME", O_NOATIME},
    {"O_NONBLOCK", O_NONBLOCK},
};

static void dump_fd_status_flags(int flags, std::ostream &stream)
{
    stream << "status_flags ";

    /* Collect flags. */
    std::vector<std::string> flag_strs;
    populate_flag_strs<int>(flags, flag_strs, status_flags, &stream,
                            false /* endl */);
}

bool fd_info(int fd, std::ostream &stream)
{
    /* Get status flags. */
    int flags = fcntl(fd, F_GETFL);
    bool success = flags != -1;

    std::string fd_string = "fd(" + std::to_string(fd) + ")";

    print_verb_name_condition(fd_string, "get status flags", success,
                              true /* show_errno */, true /* error_only */);

    /* Print info about flags. */
    if (success)
    {
        stream << fd_string << ": ";
        dump_fd_status_flags(flags, stream);

        /* Get status flags. */
        flags = fcntl(fd, F_GETFD);
        if (flags != -1 and flags & FD_CLOEXEC)
        {
            stream << ", FD_CLOEXEC=" << FD_CLOEXEC;
        }

        stream << std::endl;
    }

    return success;
}

bool fd_set_blocking_state(int fd, bool blocking)
{
    /* Get status flags. */
    int flags = fcntl(fd, F_GETFL);
    bool success = flags != -1;

    if (success)
    {
        flags = blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
        success = fcntl(fd, F_SETFL, flags) != -1;
    }

    return success;
}

} // namespace Project81
