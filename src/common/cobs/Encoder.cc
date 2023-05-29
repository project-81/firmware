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

    return result + 1;
}

bool MessageEncoder::encode(PcBufferWriter<> &writer)
{
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

            can_continue = writer.push(zero_pointer);
            if (can_continue)
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
            can_continue = writer.push(0);
            if (can_continue)
            {
                state = complete;
            }
            break;

        default:                     /* LCOV_EXCL_LINE */
            __builtin_unreachable(); /* LCOV_EXCL_LINE */
        }
    }

    return state == complete;
}

void MessageEncoder::advance_message(bool only_zero_pointer)
{
    if (not only_zero_pointer)
    {
        data++;

        assert(length > 0);
        length--;
    }

    assert(zero_pointer > 0);
    zero_pointer--;
}

MessageEncoder::ZeroPointerState MessageEncoder::pointer_kind(void)
{
    if (zero_pointer >= length)
    {
        return pointer_to_end;
    }

    return (zero_pointer == zero_pointer_max or
            last_pointer_kind == pointer_to_pointer)
               ? pointer_to_pointer
               : pointer_to_data;
}

bool MessageEncoder::handle_zero_pointer(PcBufferWriter<> &writer)
{
    bool handled = false;
    bool is_data = *data == 0;

    switch (zero_pointer_state)
    {
    case empty:
        zero_pointer = next_zero_distance(data, length, true /* skip self */);
        zero_pointer_state = pointer_kind();

        /* See if we can write the newly populated zero pointer. */
        return handle_zero_pointer(writer);

    case pointer_to_data:
        assert(is_data);
        handled = writer.push(zero_pointer);
        if (handled)
        {
            /* Only advance the pointer if this pointer is overhead. */
            advance_message(last_pointer_kind == pointer_to_pointer);
        }
        break;

    case pointer_to_pointer:
        handled = writer.push(zero_pointer);
        if (handled)
        {
            /* Only advance the pointer if we're not on a data byte. */
            advance_message(not is_data);
        }
        break;

    case pointer_to_end:
        handled = writer.push(
            (last_pointer_kind == pointer_to_pointer and not is_data)
                ? zero_pointer + 1
                : zero_pointer);

        if (handled and is_data)
        {
            /* Only advance the pointer if this pointer is overhead. */
            advance_message(last_pointer_kind == pointer_to_pointer);
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

    /*
     * Because the state machine starts by writing a zero, we
     * should only reach this location when we don't expect to
     * see a zero in the data (hence the assertion).
     */
    else
    {
        assert(*data != 0);

        can_continue = writer.push(*data);
        if (can_continue)
        {
            advance_message();

            /* Keep processing data if there's more left. */
            state = (length) ? encode_data : encode_delimeter;
        }
    }

    return can_continue;
}

} // namespace Project81::Cobs
