/* internal */
#include "boards/native/FdManager.h"

namespace Project81
{

FdManager::~FdManager()
{
    for (const auto &[name, fds] : groups)
    {
        std::cout << "Closing file-descriptor group '" << name << "'."
                  << std::endl;
        close_fds(fds);
    }
}

bool FdManager::add_file_fd(const std::string path, const std::string group,
                            const std::string mode)
{
    return get_file_fd(path, fd_group(group), mode);
}

FdMap &FdManager::fd_group(const std::string group)
{
    if (groups.try_emplace(group, FdMap()).second)
    {
        std::cout << "Created file-descriptor group '" << group << "'."
                  << std::endl;
    }
    return groups[group];
}

bool FdManager::add_fd(const std::string name, int fd, const std::string group)
{
    FdMap &fds = fd_group(group);
    bool result = not fds.contains(name);
    if (result)
    {
        fds[name] = fd;
    }
    return result;
}

int &FdManager::operator[](const std::string name)
{
    return fd_group()[name];
}

} // namespace Project81
