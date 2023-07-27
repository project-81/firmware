/*
 * input number should be in "n"
 */

#include <array>
#include <cstdint>
#include <iostream>

using namespace std;

static std::array<const char *, 8> symbols = {"n", "µ", "m", "",
                                              "k", "M", "G", "T"};

int main(void)
{
    cout << "Enter time in nanoseconds: ";

    uint64_t time_input = 0;
    cin >> time_input;

    uint64_t divisor = 1000;
    uint64_t decimal = time_input;
    uint64_t fractional = 0;

    std::size_t iteration = 0;
    while (decimal >= divisor and iteration < symbols.size())
    {
        fractional = decimal % divisor;
        decimal = decimal / divisor;
        iteration += 1;
    }

    cout << "Output is: " << decimal << "." << fractional << " "
         << symbols[iteration] << ". The end. \n";

    return 0;
}
