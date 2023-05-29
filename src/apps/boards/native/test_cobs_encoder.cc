/* toolchain */
#include <cstdio>

/* internal */
#include "common/buffer/PcBuffer.h"
#include "common/cobs/Encoder.h"

using namespace Project81;

static constexpr std::size_t buffer_size = 1024;

using PcBuf = PcBuffer<buffer_size>;

void test_zero_distances(void)
{
    uint8_t data[1024] = {};

    assert(Cobs::next_zero_distance(data, 1) == 1);
    assert(Cobs::next_zero_distance(data, 2) == 1);
    assert(Cobs::next_zero_distance(data, 3) == 1);

    data[0] = 1;

    assert(Cobs::next_zero_distance(data, 1) == 2);
    assert(Cobs::next_zero_distance(data, 2) == 2);
    assert(Cobs::next_zero_distance(data, 3) == 2);

    /* Keep the 254th byte a zero. */
    for (std::size_t i = 0; i < Cobs::zero_pointer_max - 2; i++)
    {
        data[i] = 1;
    }

    assert(Cobs::next_zero_distance(data, 1) == 2);
    assert(Cobs::next_zero_distance(data, 2) == 3);
    assert(Cobs::next_zero_distance(data, 3) == 4);
    assert(Cobs::next_zero_distance(data, 1024) == Cobs::zero_pointer_max - 1);

    /* Make the 254th byte non-zero. */
    data[Cobs::zero_pointer_max - 2] = 1;
    assert(Cobs::next_zero_distance(data, 1024) == Cobs::zero_pointer_max);
    data[Cobs::zero_pointer_max - 1] = 1;
    assert(Cobs::next_zero_distance(data, 1024) == Cobs::zero_pointer_max);
}

void dump(const uint8_t *data, std::size_t size)
{
    for (std::size_t i = 0; i < size; i++)
    {
        printf("%02x ", data[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
    printf("\n");
}

void encoder_scenario(const uint8_t *input, std::size_t input_size,
                      const uint8_t *expected, std::size_t expected_size,
                      const char *name)
{
    Cobs::MessageEncoder encoder = Cobs::MessageEncoder(input, input_size);

    /* Encode the message. */
    PcBuf buffer = PcBuf();
    assert(encoder.encode(buffer));

    /* Extract the result. */
    uint8_t output[buffer_size];
    std::size_t output_size = buffer.pop_all(output);

    printf("Output   - (%zu)\n", output_size);
    dump(output, output_size);

    printf("\nExepcted - (%zu)\n", expected_size);
    dump(expected, expected_size);

    assert(output_size == expected_size);

    /* Confirm the contents. */
    assert(std::memcmp(output, expected, expected_size) == 0);

    printf("\n(%s passed)\n", name);
}

int main(void)
{
    test_zero_distances();

    uint8_t input[buffer_size] = {0};
    uint8_t output[buffer_size] = {0};
    std::size_t input_size;
    std::size_t output_size;

    /*
     * Example 1.
     */
    input_size = 1;
    output_size = 3;
    output[0] = 1;
    output[1] = 1;
    encoder_scenario(input, input_size, output, output_size, "example 1");

    /*
     * Example 2.
     */
    input_size = 2;
    output_size = 4;
    output[2] = 1;
    encoder_scenario(input, input_size, output, output_size, "example 2");

    /*
     * Example 3.
     */
    input_size = 3;
    output_size = 5;

    input[1] = 0x11;

    output[0] = 1;
    output[1] = 2;
    output[2] = 0x11;
    output[3] = 1;
    output[4] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 3");

    /*
     * Example 4.
     */
    input_size = 4;
    output_size = 6;

    input[0] = 0x11;
    input[1] = 0x22;
    input[2] = 0;
    input[3] = 0x33;

    output[0] = 3;
    output[1] = 0x11;
    output[2] = 0x22;
    output[3] = 2;
    output[4] = 0x33;
    output[5] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 4");

    /*
     * Example 5.
     */

    input[0] = 0x11;
    input[1] = 0x22;
    input[2] = 0x33;
    input[3] = 0x44;

    output[0] = 5;
    output[1] = 0x11;
    output[2] = 0x22;
    output[3] = 0x33;
    output[4] = 0x44;
    output[5] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 5");

    /*
     * Example 6.
     */

    input[0] = 0x11;
    input[1] = 0;
    input[2] = 0;
    input[3] = 0;

    output[0] = 2;
    output[1] = 0x11;
    output[2] = 1;
    output[3] = 1;
    output[4] = 1;
    output[5] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 6");

    /*
     * Example 7.
     */
    input_size = 254;
    output_size = 256;
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i + 1;
    }

    output[0] = 255;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 1] = i + 1;
    }
    output[255] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 7");

    /*
     * Example 8.
     */
    input_size = 255;
    output_size = 257;
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i;
    }

    output[0] = 1;
    output[1] = 0xff;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 2] = i + 1;
    }
    output[256] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 8");

    /*
     * Example 9.
     */
    output_size = 258;
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i + 1;
    }

    output[0] = 0xff;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 1] = i + 1;
    }
    output[255] = 2;
    output[256] = 0xff;
    output[257] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 9");

    /*
     * Example 10.
     */
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i + 2;
    }

    output[0] = 0xff;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 1] = i + 2;
    }
    output[255] = 1;
    output[256] = 1;
    output[257] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 10");

    /*
     * Example 11.
     */
    output_size = 257;
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i + 3;
    }

    output[0] = 0xfe;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 1] = i + 3;
    }
    output[254] = 2;
    output[255] = 1;
    output[256] = 0;

    encoder_scenario(input, input_size, output, output_size, "example 11");

    /*
     * Example 8 (longer).
     */
    input_size = 256;
    output_size = 259;
    for (std::size_t i = 0; i < input_size; i++)
    {
        input[i] = i;
    }

    output[0] = 1;
    output[1] = 0xff;
    for (std::size_t i = 0; i < input_size; i++)
    {
        output[i + 2] = i + 1;
    }
    output[256] = 2;
    output[257] = 0xff;
    output[258] = 0;

    encoder_scenario(input, input_size, output, output_size,
                     "example 8 (longer)");

    return 0;
}
