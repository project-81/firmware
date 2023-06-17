#pragma once

/* toolchain */
#include <iostream>

/* internal */
#include "boards/native/file_descriptors.h"

namespace Project81
{

class FdManager
{
  public:
    static constexpr std::string default_group = "root";

    using FdGroup = std::map<std::string, FdMap>;

    FdManager() : groups()
    {
    }

    ~FdManager();

    bool add_file_fd(const std::string path,
                     const std::string group = default_group,
                     const std::string mode = default_open_mode);

    FdMap &fd_group(const std::string group = default_group);

    bool add_fd(const std::string name, int fd,
                const std::string group = default_group);

    int &operator[](const std::string name);

  protected:
    FdGroup groups;
};

} // namespace Project81
