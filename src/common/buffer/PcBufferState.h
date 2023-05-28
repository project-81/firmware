#pragma once

/* toolchain */
#include <cstdint>

/* internal */
#include "common/buffer/PcBufferStats.h"

namespace Project81
{

class PcBufferState
{
  public:
    PcBufferState(std::size_t _size)
        : data(0), space(_size), size(_size), stats()
    {
    }

    void reset()
    {
        /* Reset state. */
        data = 0;
        space = size;

        /* Reset stats. */
        stats.reset();
    }

    inline bool has_enough_space(std::size_t count)
    {
        return space >= count;
    }

    bool increment_data(bool drop, std::size_t count = 1)
    {
        bool result = has_enough_space(count);

        if (result)
        {
            data += count;
            if (data > stats.high_watermark)
            {
                stats.high_watermark = data;
            }
            space -= count;

            stats.write_count += count;
        }
        else if (drop)
        {
            stats.write_dropped += count;
        }

        return result;
    }

    inline bool has_enough_data(std::size_t count)
    {
        return data >= count;
    }

    bool decrement_data(std::size_t count = 1)
    {
        bool result = has_enough_data(count);

        if (result)
        {
            data -= count;
            space += count;

            stats.read_count += count;
        }

        return result;
    }

    void poll_stats(PcBufferStats &update)
    {
        update = stats;
        stats.reset();
    }

    std::size_t data;
    std::size_t space;

    const std::size_t size;

  protected:
    PcBufferStats stats;
};

} // namespace Project81
