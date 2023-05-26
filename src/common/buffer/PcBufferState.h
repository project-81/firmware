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

    bool increment_data(bool drop)
    {
        bool result = space > 0;

        if (result)
        {
            data++;
            if (data > stats.high_watermark)
            {
                stats.high_watermark = data;
            }
            space--;

            stats.write_count++;
        }
        else if (drop)
        {
            stats.write_dropped++;
        }

        return result;
    }

    bool decrement_data()
    {
        bool result = data > 0;

        if (result)
        {
            data--;
            space++;

            stats.read_count++;
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
