/* linux */
#include <sys/epoll.h>

/* toolchain */
#include <fstream>
#include <iostream>

/* third-party */
#include "nlohmann/json.hpp"

/* internal */
#include "boards/native/Viktor.h"
#include "boards/native/file_descriptors.h"
#include "boards/native/text.h"

using namespace Project81;

namespace fs = std::filesystem;
using json = nlohmann::json;

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

    FdMap fds;
    bool initialization_errors = false;

    /*
     * Open serial ports.
     */
    for (auto &tty : data["ttys"].items())
    {
        initialization_errors |= !get_rw_file_fd(tty.value(), fds);
    }

    /*
     * Create the event-poll interface.
     */
    int epoll = epoll_create1(EPOLL_CLOEXEC);
    if (epoll < 0)
    {
        initialization_errors = true;
    }
    else
    {
        fds["epoll"] = epoll;
    }
    print_verb_name_condition("epoll", "create", !initialization_errors);

    int result = 1;

    /*
     * Start the application if initialization succeeds.
     */
    if (not initialization_errors)
    {
        Viktor app(fds);
        result = app.run();
    }

    if (result)
    {
        std::cerr << "Initialization failed." << std::endl;
    }
    else
    {
        std::cout << "Nominal exit." << std::endl;
    }

    return result;
}
