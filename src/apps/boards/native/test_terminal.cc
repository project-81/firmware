/* linux */
#include <fcntl.h>
#include <sys/epoll.h>

/* toolchain */
#include <cassert>
#include <cstdio>
#include <string>

/* internal */
#include "boards/native/EpollManager.h"
#include "boards/native/Termios.h"
#include "boards/native/file_descriptors.h"

using namespace Project81;

using Epoll = EpollManager<>;

bool echo_handler(int fd, uint32_t events)
{
    bool result = events & EPOLLIN;

    /* Read from the file descriptor. */
    if (result)
    {
        char data = '\0';
        while (read(fd, &data, 1) == 1 && result)
        {
            result = data != 'q';

            std::cout << int(data);
            if (iscntrl(data))
            {
                std::cout << " <cntrl>";
            }
            else
            {
                std::cout << " (" << data << ")";
            }
            std::cout << std::endl;
        }
    }

    return result;
}

void initialize(Termios &terminal, Epoll &epoll)
{
    /* Turn input echo and canonical mode off. */
    assert(terminal.set_echo(false));
    assert(terminal.set_canonical(false));

    /* Add stdout/stdin. */
    assert(epoll.add(terminal.fd, EPOLLIN));

    /* Add an echo handler for stdin. */
    assert(epoll.event_handler(terminal.fd, echo_handler));
}

bool app(Epoll &epoll)
{
    int event_count = 0;

    while (epoll.has_handlers())
    {
        event_count += epoll.run();
    }

    return event_count > 0;
}

int main(void)
{
    /* Disable stdout (and stdin) buffering and make it non-blocking. */
    setbuf(stdout, nullptr);
    // setbuf(stdin, nullptr);
    setvbuf(stdin, NULL, _IONBF, 0);
    assert(fd_set_blocking_state(fileno(stdout)));
    // assert(fd_set_blocking_state(fileno(stdin)));

    int stdin_fd = fileno(stdin);

    /* Dump standard input/output */
    fd_info(stdin_fd);
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
        Termios term(stdin_fd);
        Epoll epoll;

        initialize(term, epoll);
        result = app(epoll);
    }
    print_verb_name_condition("app", "run", result);
    return result ? 0 : 1;
}
