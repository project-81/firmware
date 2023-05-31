/* internal */
#include "common/cobs/Encoder.h"

namespace Project81::Cobs
{

uint8_t next_zero_distance(const uint8_t *data, std::size_t length,
                           bool skip_self)
{
    uint8_t result = 0;

    /*
     * An option for ensuring that a zero at the current buffer position
     * doesn't result in this also producing 1.
     */
    if (skip_self)
    {
        assert(length);

        data++;
        length--;
    }

    while (result < zero_pointer_max - 1 and result < length)
    {
        if (data[result] == 0)
        {
            break;
        }
        result++;
    }

    /* Return 'distance', not index. */
    return result + 1;
}

bool MessageEncoder::stage(const uint8_t *_data, std::size_t _length)
{
    /*
     * Can only stage a message if the previous encoding is complete, or
     * the encoder hasn't been initialized yet.
     */
    bool result = state == not_initialized or state == complete;

    if (result)
    {
        data = _data;
        length = _length;
        state = start;
    }

    return result;
}

bool MessageEncoder::stage(const char *_data, std::size_t _length)
{
    return stage((const uint8_t *)_data, _length);
}

bool MessageEncoder::encode(PcBufferWriter<> &writer)
{
    /* Attempting to encode with no staged message is a usage bug. */
    assert(state != not_initialized);

    bool can_continue = true;

    while (state != complete and can_continue)
    {
        switch (state)
        {
        case start:
            /* Initialize the first zero pointer if necessary. */
            if (zero_pointer == 0)
            {
                zero_pointer = next_zero_distance(data, length);
            }

            if ((can_continue = writer.push(zero_pointer)))
            {
                /* Ensure the last-pointer kind is set correctly. */
                last_pointer_kind = (zero_pointer == zero_pointer_max)
                                        ? pointer_to_pointer
                                        : pointer_to_data;

                /* Don't move the message buffer forward. */
                advance_message(true);
                state = encode_data;
            }

            break;

        case encode_zero:
            can_continue = handle_encode_zero(writer);
            break;

        case encode_data:
            can_continue = handle_encode_data(writer);
            break;

        case encode_delimeter:
            if ((can_continue = writer.push(0)))
            {
                state = complete;
            }
            break;

        /*
         * Not being initialized is a bug, and the complete state breaks us out
         * of the loop.
         */
        default:                     /* LCOV_EXCL_LINE */
            __builtin_unreachable(); /* LCOV_EXCL_LINE */
        }
    }

    bool result = state == complete;

    if (result)
    {
        assert(zero_pointer == 0);
        assert(length == 0);

        state = not_initialized;
    }

    return result;
}

void MessageEncoder::advance_message(bool only_zero_pointer, std::size_t count)
{
    if (not only_zero_pointer)
    {
        data += count;

        assert(length >= count);
        length -= count;
    }

    assert(zero_pointer >= count);
    zero_pointer -= count;
}

MessageEncoder::ZeroPointerState MessageEncoder::pointer_kind(void)
{
    /*
     * For a zero pointer to point to end-of-frame, it must be longer than the
     * length.
     */
    if (zero_pointer > length)
    {
        return pointer_to_end;
    }

    return (zero_pointer == zero_pointer_max) ? pointer_to_pointer
                                              : pointer_to_data;
}

bool MessageEncoder::handle_zero_pointer(PcBufferWriter<> &writer)
{
    bool handled = false;

    bool is_overhead = last_pointer_kind == pointer_to_pointer;

    switch (zero_pointer_state)
    {
    case empty:
        zero_pointer =
            next_zero_distance(data, length, !is_overhead /* skip self */);
        zero_pointer_state = pointer_kind();

        /* See if we can write the newly populated zero pointer. */
        return handle_zero_pointer(writer);

    case pointer_to_data:
    case pointer_to_pointer:
    case pointer_to_end:
        /*
         * A 'pointer to the end' should never be followed by another zero
         * pointer.
         */
        assert(last_pointer_kind != pointer_to_end);

        if ((handled = writer.push(zero_pointer)))
        {
            /* If this isn't an overhead pointer, we should be on a zero. */
            assert(is_overhead or *data == 0);

            /* Only advance the pointer if this pointer is overhead. */
            advance_message(is_overhead);
        }
        break;
    }

    /* Reset the state machine for the next invocation. */
    if (handled)
    {
        last_pointer_kind = zero_pointer_state;
        zero_pointer_state = empty;
    }

    return handled;
}

bool MessageEncoder::handle_encode_zero(PcBufferWriter<> &writer)
{
    bool can_continue = handle_zero_pointer(writer);

    if (can_continue)
    {
        /* If there's no more data, encode the delimeter. */
        state = (length) ? encode_data : encode_delimeter;
    }

    return can_continue;
}

bool MessageEncoder::handle_encode_data(PcBufferWriter<> &writer)
{
    bool can_continue = true;

    /*
     * If we've advanced towards an expected zero (or extra zero
     * pointer), go handle it.
     */
    if (zero_pointer == 0)
    {
        state = encode_zero;
    }

    /* Write data until the next zero. */
    else if ((can_continue = writer.push_n(data, zero_pointer)))
    {
        advance_message(false, zero_pointer);

        /*
         * Keep processing data if there's more left. Since we processed
         * the entire data chunk (until the next zero), we know a zero
         * comes next.
         */
        state = (length) ? encode_zero : encode_delimeter;
    }

    return can_continue;
}

} // namespace Project81::Cobs
