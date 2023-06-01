#pragma once

/* internal */
#include "boards/native/file_descriptors.h"

namespace Project81
{

class Viktor
{
  public:
    Viktor(FdMap &_fds) : fds(_fds)
    {
        epoll_fd = fds["epoll"];
    }

    ~Viktor()
    {
        close_fds(fds);
    }

    int run(void)
    {
        /* Start adding stuff to epoll. */
        (void)epoll_fd;
        // epoll_ctl(epoll, 0, );

        return 0;
    }

  protected:
    FdMap &fds;
    int epoll_fd;
};

} // namespace Project81
