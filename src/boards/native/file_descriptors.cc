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

static void dump_fd_status_flags(int flags, std::ostream &stream)
{
    /* Collect flags. */
    std::vector<std::string> flag_strs;

    /*
     * Access modes.
     */
    if (flags & O_RDONLY)
    {
        flag_strs.push_back("O_RDONLY=" + std::to_string(O_RDONLY));
    }
    if (flags & O_WRONLY)
    {
        flag_strs.push_back("O_WRONLY=" + std::to_string(O_WRONLY));
    }
    if (flags & O_RDWR)
    {
        flag_strs.push_back("O_RDWR=" + std::to_string(O_RDWR));
    }

    /*
     * Open-time flags.
     */
    if (flags & O_CREAT)
    {
        flag_strs.push_back("O_CREAT=" + std::to_string(O_CREAT));
    }
    if (flags & O_EXCL)
    {
        flag_strs.push_back("O_EXCL=" + std::to_string(O_EXCL));
    }
    if (flags & O_NOCTTY)
    {
        flag_strs.push_back("O_NOCTTY=" + std::to_string(O_NOCTTY));
    }
    if (flags & O_TRUNC)
    {
        flag_strs.push_back("O_TRUNC=" + std::to_string(O_TRUNC));
    }
    if (flags & O_APPEND)
    {
        flag_strs.push_back("O_APPEND=" + std::to_string(O_APPEND));
    }
    if (flags & O_ASYNC)
    {
        flag_strs.push_back("O_ASYNC=" + std::to_string(O_ASYNC));
    }
    if (flags & O_DIRECT)
    {
        flag_strs.push_back("O_DIRECT=" + std::to_string(O_DIRECT));
    }
    if (flags & O_NOATIME)
    {
        flag_strs.push_back("O_NOATIME=" + std::to_string(O_NOATIME));
    }
    if (flags & O_NONBLOCK)
    {
        flag_strs.push_back("O_NONBLOCK=" + std::to_string(O_NONBLOCK));
    }

    stream << "status_flags=" << flags;

    for (auto &flag_str : flag_strs)
    {
        stream << ", " << flag_str;
    }
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
