#pragma once

/* toolchain */
#include <iostream>
#include <map>
#include <string>

namespace Project81
{

using FdMap = std::map<std::string, int>;

void close_fds(const FdMap &fds);

static constexpr std::string default_open_mode = "r+";

bool get_file_fd(const std::string path, FdMap &fds,
                 const std::string mode = default_open_mode);

bool fd_info(int fd, std::ostream &stream = std::cout);

bool fd_set_blocking_state(int fd, bool blocking = false);

} // namespace Project81
