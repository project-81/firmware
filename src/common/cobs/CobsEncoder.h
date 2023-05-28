#pragma once

/* toolchain */
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>

/* internal */
#include "common/buffer/PcBufferInterface.h"

namespace Project81
{

class CobsEncoder
{
    static constexpr std::size_t max_cobs_chunk =
        std::numeric_limits<uint8_t>::max();

    using cobs_chunk_array = std::array<uint8_t, max_cobs_chunk>;

  public:
    static uint8_t handle_next_zero(const uint8_t *message,
                                    uint8_t &message_index, std::size_t length,
                                    cobs_chunk_array &output);

    CobsEncoder(const uint8_t *_message, std::size_t _length)
        : message(_message), length(_length), output(), to_write(0)
    {
    }

    void stage_message(const uint8_t *_message, std::size_t _length)
    {
        /*
         * Don't swap in a new, underlying message until the current one has
         * been encoded.
         */
        assert(done());
        message = _message;
        length = _length;
    }

    inline bool done(void)
    {
        return length == 0 and to_write == 0;
    }

    bool encode(PcBufferWriter<> &writer);

  protected:
    const uint8_t *message;
    std::size_t length;

    cobs_chunk_array output;
    uint8_t to_write;
};

} // namespace Project81
