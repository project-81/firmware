#pragma once

/* linux */
#include <termios.h>

/* toolchain */
#include <iostream>

namespace Project81
{

class Termios
{
  public:
    Termios(int _fd);
    ~Termios();

    void info(std::ostream &stream = std::cout);

  protected:
    int fd;
    bool valid;
    struct termios original;
    struct termios current;
};

} // namespace Project81
