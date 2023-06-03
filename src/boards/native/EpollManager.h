#pragma once

/* linux */
#include <sys/epoll.h>

/* toolchain */
#include <array>
#include <iostream>
#include <unistd.h>

/* internal */
#include "boards/native/text.h"

namespace Project81
{

template <int max_events = 64> class EpollManager
{
  public:
    enum operation : int
    {
        add = EPOLL_CTL_ADD,
        mod = EPOLL_CTL_MOD,
        del = EPOLL_CTL_DEL,
    };

    EpollManager() : epoll(epoll_create1(0)), valid(true), events()
    {
        valid = epoll != -1;
        print_verb_name_condition("epoll", "create", valid);
    }

    ~EpollManager()
    {
        valid = close(epoll) == 0;
        print_verb_name_condition("epoll", "close", valid);
    }

    bool ctl(operation op, int fd, uint32_t events)
    {
        bool result = false;
        if (valid)
        {
            struct epoll_event event = {.events = events, .data = {.fd = fd}};
            result = epoll_ctl(epoll, op, fd, &event) == 0;
        }

        std::string fd_string = "poll fd(" + std::to_string(fd) + ")";
        print_verb_name_condition(fd_string,
                                  op == add   ? "add"
                                  : op == mod ? "mod"
                                              : "del",
                                  result);

        return result;
    }

    int wait(int timeout)
    {
        return epoll_wait(epoll, events.data(), max_events, timeout);
    }

    bool run(void)
    {
        if (!valid)
        {
            return false;
        }

        int result = wait(-1);
        std::cout << result << std::endl;

        return true;
    }

  protected:
    int epoll;
    bool valid;
    std::array<struct epoll_event, max_events> events;
};

} // namespace Project81
