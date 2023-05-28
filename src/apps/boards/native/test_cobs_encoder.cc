/* toolchain */
#include <cstring>

/* internal */
#include "common/cobs/CobsEncoder.h"

using namespace Project81;

static constexpr std::size_t array_size = 1024;

int main(void)
{
    uint8_t data[array_size] = {};
    uint8_t message_index = 0;
    std::size_t length = 0;
    std::array<uint8_t, 255> output;

    assert(CobsEncoder::handle_next_zero(data, message_index, length,
                                         output) == 0);
    assert(message_index == 0);

    /*
     * Only the zero pointer should get written if the data starts with zero.
     */
    data[0] = 0;
    length = 1;
    assert(CobsEncoder::handle_next_zero(data, message_index, length,
                                         output) == 1);
    assert(output[0] == 1);
    assert(message_index == 1);

    /*
     * The zero pointer and the single data byte should get written if there's
     * a single, non-zero data byte.
     */
    data[0] = 1;
    assert(CobsEncoder::handle_next_zero(data, message_index, length,
                                         output) == 2);
    assert(output[0] == 2);
    assert(output[1] == 1);
    assert(message_index == 1);

    /*
     *
     */
    std::memset(data, 1, array_size);
    length = array_size;
    assert(CobsEncoder::handle_next_zero(data, message_index, length,
                                         output) == 255);
    assert(output[0] == 255);
    for (std::size_t i = 1; i < 255; i++)
    {
        assert(output[i] == 1);
    }
    assert(message_index == 254);

    return 0;
}
