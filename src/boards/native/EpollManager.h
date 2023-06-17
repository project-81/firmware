#pragma once

/* linux */
#include <sys/epoll.h>

/* toolchain */
#include <array>
#include <functional>
#include <iostream>
#include <unistd.h>

/* internal */
#include "boards/native/text.h"

namespace Project81
{

template <int max_events = 64> class EpollManager
{
  public:
    using EventCallback = std::function<bool(int, uint32_t)>;

    EpollManager() : epoll(epoll_create1(0)), valid(true), events(), handlers()
    {
        valid = epoll != -1;
        print_verb_name_condition("epoll", "create", valid);
    }

    ~EpollManager()
    {
        valid = close(epoll) == 0;
        print_verb_name_condition("epoll", "close", valid);
    }

    bool event_handler(int fd, EventCallback callback)
    {
        bool result = not handlers.contains(fd);
        if (result)
        {
            handlers.emplace(fd, callback);
        }
        return result;
    }

    bool has_handlers(void)
    {
        return not handlers.empty();
    }

    bool add(int fd, uint32_t events)
    {
        return ctl(op_add, fd, events);
    }

    bool mod(int fd, uint32_t events)
    {
        return ctl(op_mod, fd, events);
    }

    bool del(int fd, uint32_t events)
    {
        return ctl(op_del, fd, events);
    }

    int run(int timeout = -1)
    {
        int result = wait(timeout);

        for (int i = 0; i < result; i++)
        {
            epoll_event &event = events[i];
            int fd = event.data.fd;

            if (handlers.contains(fd))
            {
                if (!handlers[fd](fd, event.events))
                {
                    handlers.erase(fd);
                }
            }
        }

        return result;
    }

  protected:
    int epoll;
    bool valid;
    std::array<epoll_event, max_events> events;
    std::map<int, EventCallback> handlers;

    enum operation : int
    {
        op_add = EPOLL_CTL_ADD,
        op_mod = EPOLL_CTL_MOD,
        op_del = EPOLL_CTL_DEL,
    };

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
                                  op == op_add   ? "add"
                                  : op == op_mod ? "mod"
                                                 : "del",
                                  result);

        return result;
    }

    int wait(int timeout)
    {
        return epoll_wait(epoll, events.data(), max_events, timeout);
    }
};

} // namespace Project81
