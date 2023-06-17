/* linux */
#include <sys/epoll.h>

/* toolchain */
#include <fstream>
#include <iostream>

/* third-party */
#include "nlohmann/json.hpp"

/* internal */
#include "boards/native/FdManager.h"
#include "boards/native/Viktor.h"
#include "boards/native/file_descriptors.h"
#include "boards/native/text.h"

using namespace Project81;

using json = nlohmann::json;

static constexpr std::string serial_key = "ttys";

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        std::cerr << "No JSON configuration specified." << std::endl;
        return 1;
    }

    /* Load configuration. */
    std::cout << "Loading configuration '" << argv[1] << "'." << std::endl;
    json data = json::parse(std::ifstream(argv[1]));

    /* Dump configuration to output. */
    std::cout << data << std::endl;

    int result = 1;

    {
        bool initialization_errors = false;
        FdManager fds;

        /*
         * Open serial ports.
         */
        for (auto &tty : data[serial_key].items())
        {
            initialization_errors |=
                not fds.add_file_fd(tty.value(), serial_key);
        }

        /*
         * Open TCP server port.
         */

        /*
         * Open TCP client ports.
         */
        // future

        /*
         * Create the event-poll interface.
         */
        int epoll = epoll_create1(EPOLL_CLOEXEC);
        if (epoll == -1)
        {
            initialization_errors = true;
        }
        else
        {
            fds.add_fd("epoll", epoll);
        }
        print_verb_name_condition("epoll", "create", epoll != -1);

        /*
         * Start the application if initialization succeeds.
         */
        print_verb_name_condition("viktor", "initialize",
                                  not initialization_errors);
        if (not initialization_errors)
        {
            Viktor app(fds);
            result = app.run();
        }
    }

    print_verb_name_condition("viktor", "run", result == 0);
    return result;
}
