#pragma once

/* toolchain */
#include <cassert>
#include <cstdint>
#include <limits>

/* internal */
#include "common/buffer/PcBufferInterface.h"

namespace Project81::Cobs
{

static constexpr uint8_t zero_pointer_max =
    std::numeric_limits<uint8_t>::max();

uint8_t next_zero_distance(const uint8_t *data, std::size_t length,
                           bool skip_self = false);

class MessageEncoder
{
  public:
    MessageEncoder(const uint8_t *_data, std::size_t _length)
        : data(_data), length(_length)
    {
    }

    bool encode(PcBufferWriter<> &writer);

  protected:
    const uint8_t *data;
    std::size_t length;

    /*
     * Always start by encoding the first zero.
     */
    enum State
    {
        start,
        encode_zero,
        encode_data,
        encode_delimeter,
        complete,
    };
    State state = start;

    /*
     * The first zero starts out with an unknown value.
     */
    enum ZeroPointerState
    {
        empty,
        pointer_to_data,
        pointer_to_end,
        pointer_to_pointer,
    };
    ZeroPointerState zero_pointer_state = empty;
    ZeroPointerState last_pointer_kind = empty;

    /*
     * When a zero needs to be written: holds the distance to the next zero.
     *
     * Decremented every time message bytes are processed, until it reaches
     * zero, indicating that it should be re-computed and this new value should
     * be written to the output stream..
     */
    uint8_t zero_pointer = 0;

    void advance_message(bool only_zero_pointer = false);

    bool handle_zero_pointer(PcBufferWriter<> &writer);
    bool handle_encode_zero(PcBufferWriter<> &writer);
    bool handle_encode_data(PcBufferWriter<> &writer);

    ZeroPointerState pointer_kind(void);
};

}; // namespace Project81::Cobs
