#pragma once

/* internal */
#include "boards/native/FdManager.h"
#include "boards/native/file_descriptors.h"

namespace Project81
{

class Viktor
{
  public:
    Viktor(FdManager &_fds) : fds(_fds)
    {
        epoll = fds["epoll"];
    }

    int run(void)
    {
        /* Start adding stuff to epoll. */
        (void)epoll;
        std::cout << epoll << std::endl;
        // epoll_ctl(epoll, 0, );

        return 0;
    }

  protected:
    FdManager &fds;
    int epoll;
};

} // namespace Project81
