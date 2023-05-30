/* toolchain */
// #include <cstring>

/* internal */
#include "common/buffer/PcBuffer.h"
#include "common/cobs/Decoder.h"

using namespace Project81;

static constexpr std::size_t message_mtu = 1024;

using Decoder = Cobs::MessageDecoder<message_mtu>;
using PcBuf = PcBuffer<message_mtu * 2>;

int main(void)
{
    Decoder decoder = Decoder();

    int messages;
    uint8_t message[message_mtu];
    std::size_t message_size;

    decoder.set_message_callback(
        [&messages, &message, &message_size](
            const std::array<uint8_t, message_mtu> &data, std::size_t size) {
            std::memcpy(message, data.data(), size);
            message_size = size;
            messages++;
        });

    PcBuf buffer = PcBuf();

    /*
     * Example 1.
     */
    messages = 0;
    std::memset(message, 0xff, message_mtu);

    /* Message is just zero. */
    assert(buffer.push(1));
    assert(buffer.push(1));
    assert(buffer.push(0));

    decoder.dispatch(buffer);

    /* We expect one message of size one, data is zero. */
    assert(messages == 1);
    assert(message_size == 1);
    assert(message[0] == 0);

    /*
     * Example 2.
     */
    messages = 0;
    std::memset(message, 0xff, message_mtu);

    // TODO

    return 0;
}
