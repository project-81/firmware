/* linux */
#include <fcntl.h>
#include <sys/epoll.h>

/* toolchain */
#include <cassert>
#include <cstdio>
#include <string>

/* internal */
#include "boards/native/EpollManager.h"
#include "boards/native/file_descriptors.h"

using namespace Project81;

using Epoll = EpollManager<>;

int main(void)
{
    /* Disable stdout (and stdin) buffering and make it non-blocking. */
    setbuf(stdout, nullptr);
    // setbuf(stdin, nullptr);
    setvbuf(stdin, NULL, _IONBF, 0);
    assert(fd_set_blocking_state(fileno(stdout)));
    // assert(fd_set_blocking_state(fileno(stdin)));

    /* Dump standard input/output */
    fd_info(fileno(stdin));
    fd_info(fileno(stdout));
    fd_info(fileno(stderr));

    const std::string message = "Hello, world! Hello, world! Hello, world!\n";

    for (int i = 0; i < 10; i++)
    {
        if (fputs(message.data(), stdout) < 0)
        {
            return 1;
        }
    }

    bool result = false;
    {
        Epoll epoll;

        /* Add stdout/stdin. */
        assert(epoll.ctl(Epoll::add, fileno(stdin), EPOLLIN));

        result = epoll.run();
    }
    print_verb_name_condition("app", "run", result);
    return result ? 0 : 1;
}
