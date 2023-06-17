/* toolchain */
#include <cstdio>

/* internal */
#include "boards/native/Termios.h"

using namespace Project81;

int main(void)
{
    {
        Termios term(fileno(stdin));
        term.info();
    }
    return 0;
}
