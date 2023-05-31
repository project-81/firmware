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
    MessageEncoder(const void *_data = nullptr, std::size_t _length = 0)
        : data(nullptr), length(0), state(not_initialized)
    {
        /* Advance to the start state if we were constructed with real data. */
        if (_data and _length)
        {
            assert(stage((const uint8_t *)_data, _length));
        }
    }

    /* Attempt to stage a message for encoding. */
    bool stage(const uint8_t *_data, std::size_t _length);
    bool stage(const char *_data, std::size_t _length);

    /*
     * Make as much encoding progress as possible. Returns true when the staged
     * message is completely encoded.
     *
     * Returns true only once per message.
     *
     * Returns false when the message isn't fully encoded, but the buffer
     * writer is out of space. This allows better de-coupling of an underlying
     * buffer-writer's storage size, and arbitrarily large messages to encode.
     *
     * An assertion fails if a message is not currently staged.
     */
    bool encode(PcBufferWriter<> &writer);

  protected:
    const uint8_t *data;
    std::size_t length;

    /*
     * Always start by encoding the first zero.
     */
    enum State
    {
        not_initialized,
        start,
        encode_zero,
        encode_data,
        encode_delimeter,
        complete,
    };
    State state;

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

    void advance_message(bool only_zero_pointer = false,
                         std::size_t count = 1);

    bool handle_zero_pointer(PcBufferWriter<> &writer);
    bool handle_encode_zero(PcBufferWriter<> &writer);
    bool handle_encode_data(PcBufferWriter<> &writer);

    ZeroPointerState pointer_kind(void);
};

}; // namespace Project81::Cobs
