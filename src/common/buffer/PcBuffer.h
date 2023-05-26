#pragma once

/* toolchain */
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

/* internal */
#include "common/buffer/PcBufferState.h"

namespace Project81
{

template <std::size_t depth, typename element_t = std::uint8_t> class PcBuffer
{
  public:
    PcBuffer() : state(depth), buffer(), write_cursor(0), read_cursor(0)
    {
    }

    bool empty()
    {
        return state.data == 0;
    }

    bool full()
    {
        return state.space == 0;
    }

    void clear()
    {
        /* Reset state. */
        state.reset();
    }

    bool pop(element_t &elem)
    {
        bool result = state.decrement_data();

        if (result)
        {
            elem = buffer[read_cursor++ % depth];
        }

        return result;
    }

    bool pop_n(element_t *elem_array, std::size_t count)
    {
        bool result = false;

        if (state.data >= count)
        {
            for (std::size_t i = 0; i < count; i++)
            {
                pop(elem_array[i]);
            }
            result = true;
        }

        return result;
    }

    std::size_t pop_all(element_t *elem_array)
    {
        std::size_t result = state.data;
        pop_n(elem_array, result);
        return result;
    }

    bool push(element_t elem, bool drop = false)
    {
        bool result = state.increment_data(drop);

        if (result)
        {
            buffer[write_cursor++ % depth] = elem;
        }

        return result;
    }

    bool push_n(const element_t *elem_array, std::size_t count)
    {
        bool result = false;

        if (state.space >= count)
        {
            for (std::size_t i = 0; i < count; i++)
            {
                push(elem_array[i]);
            }
            result = true;
        }

        return result;
    }

    PcBufferState state;

  protected:
    std::array<element_t, depth> buffer;

    std::size_t write_cursor;
    std::size_t read_cursor;
};

} // namespace Project81
