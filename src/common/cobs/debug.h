#pragma once

/* toolchain */
#include <cassert>
#include <cstdio>
#include <cstring>

/* internal */
#include "common/buffer/debug.h"
#include "common/cobs/Decoder.h"

namespace Project81
{

template <std::size_t _message_mtu>
void register_message_validator(Cobs::MessageDecoder<_message_mtu> &decoder,
                                bool &message_seen, const uint8_t *&expected,
                                std::size_t &expected_size)
{
    decoder.set_message_callback(
        [&expected, &expected_size, &message_seen](
            const std::array<uint8_t, _message_mtu> &data, std::size_t size) {
            printf("\nOutput   - (%zu)\n", size);
            dump(data.data(), size);
            printf("Exepcted - (%zu)\n", expected_size);
            dump(expected, expected_size);

            /* Only expect to see one message. */
            assert(not message_seen);

            assert(size == expected_size);
            assert(std::memcmp(data.data(), expected, size) == 0);

            message_seen = true;
        });
}

} // namespace Project81
