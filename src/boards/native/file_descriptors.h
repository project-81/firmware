#pragma once

/* toolchain */
#include <map>
#include <string>

namespace Project81
{

using FdMap = std::map<std::string, int>;

void close_fds(const FdMap &fds);

bool get_rw_file_fd(const std::string path, FdMap &fds);

} // namespace Project81
