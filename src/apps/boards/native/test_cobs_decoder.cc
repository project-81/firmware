/* internal */
#include "common/buffer/PcBuffer.h"
#include "common/cobs/debug.h"

using namespace Project81;

static constexpr std::size_t message_mtu = 1024;

using Decoder = Cobs::MessageDecoder<message_mtu>;
using PcBuf = PcBuffer<message_mtu * 2>;

void decoder_scenario(const uint8_t *message, std::size_t message_size,
                      const uint8_t *expected, std::size_t expected_size)
{
    Decoder decoder = Decoder();

    bool message_seen = false;

    register_message_validator<message_mtu>(decoder, message_seen, expected,
                                            expected_size);

    /* Write the message into the buffer. */
    PcBuf buffer = PcBuf();
    assert(buffer.push_n(message, message_size));

    /* Decode the message. */
    decoder.dispatch(buffer);

    /* confirm that the message was actually seen. */
    assert(message_seen);

    message_seen = false;

    /* Decode again, but one byte at a time. */
    for (std::size_t i = 0; i < message_size; i++)
    {
        assert(not message_seen);
        decoder.dispatch(buffer);
        assert(buffer.push(message[i]));
    }
    decoder.dispatch(buffer);
    assert(message_seen);
}

int main(void)
{
    uint8_t message[message_mtu];
    uint8_t expected[message_mtu];

    /*
     * See:
     * https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing#Encoding_examples
     */

    /*
     * Example 1.
     */
    message[0] = 1;
    message[1] = 1;
    message[2] = 0;
    expected[0] = 0;
    decoder_scenario(message, 3, expected, 1);

    /*
     * Example 2.
     */
    message[2] = 1;
    message[3] = 0;
    expected[1] = 0;
    decoder_scenario(message, 4, expected, 2);

    /*
     * Example 3.
     */
    message[0] = 1;
    message[1] = 2;
    message[2] = 0x11;
    message[3] = 1;
    message[4] = 0;
    expected[0] = 0;
    expected[1] = 0x11;
    expected[2] = 0;
    decoder_scenario(message, 5, expected, 3);

    /*
     * Example 4.
     */
    message[0] = 3;
    message[1] = 0x11;
    message[2] = 0x22;
    message[3] = 2;
    message[4] = 0x33;
    message[5] = 0;
    expected[0] = 0x11;
    expected[1] = 0x22;
    expected[2] = 0;
    expected[3] = 0x33;
    decoder_scenario(message, 6, expected, 4);

    /*
     * Example 5.
     */
    message[0] = 5;
    message[1] = 0x11;
    message[2] = 0x22;
    message[3] = 0x33;
    message[4] = 0x44;
    message[5] = 0;
    expected[0] = 0x11;
    expected[1] = 0x22;
    expected[2] = 0x33;
    expected[3] = 0x44;
    decoder_scenario(message, 6, expected, 4);

    /*
     * Example 6.
     */
    message[0] = 2;
    message[1] = 0x11;
    message[2] = 1;
    message[3] = 1;
    message[4] = 1;
    message[5] = 0;
    expected[0] = 0x11;
    expected[1] = 0;
    expected[2] = 0;
    expected[3] = 0;
    decoder_scenario(message, 6, expected, 4);

    /*
     * Example 7.
     */

    message[0] = 0xff;
    for (std::size_t i = 0; i < 254; i++)
    {
        message[i + 1] = i + 1;
    }
    message[255] = 0;

    for (std::size_t i = 0; i < 254; i++)
    {
        expected[i] = i + 1;
    }

    decoder_scenario(message, 256, expected, 254);

    /*
     * Example 8.
     */

    message[0] = 1;
    message[1] = 0xff;
    for (std::size_t i = 0; i < 255; i++)
    {
        message[i + 2] = i + 1;
    }
    message[256] = 0;

    for (std::size_t i = 0; i < 255; i++)
    {
        expected[i] = i;
    }

    decoder_scenario(message, 257, expected, 255);

    /*
     * Example 9.
     */

    message[0] = 0xff;
    for (std::size_t i = 0; i < 255; i++)
    {
        message[i + 1] = i + 1;
    }
    message[255] = 2;
    message[256] = 0xff;
    message[257] = 0;

    for (std::size_t i = 0; i < 255; i++)
    {
        expected[i] = i + 1;
    }

    decoder_scenario(message, 258, expected, 255);

    /*
     * Example 10.
     */

    message[0] = 0xff;
    for (std::size_t i = 0; i < 255; i++)
    {
        message[i + 1] = i + 2;
    }
    message[255] = 1;
    message[256] = 1;
    message[257] = 0;

    for (std::size_t i = 0; i < 255; i++)
    {
        expected[i] = i + 2;
    }

    decoder_scenario(message, 258, expected, 255);

    /*
     * Example 11.
     */

    message[0] = 0xfe;
    for (std::size_t i = 0; i < 255; i++)
    {
        message[i + 1] = i + 3;
    }
    message[254] = 2;
    message[255] = 1;
    message[256] = 0;

    for (std::size_t i = 0; i < 255; i++)
    {
        expected[i] = i + 3;
    }

    decoder_scenario(message, 257, expected, 255);

    return 0;
}
