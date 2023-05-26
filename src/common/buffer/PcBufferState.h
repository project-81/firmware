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

    void increment_data()
    {
        data++;
        if (data > stats.high_watermark)
        {
            stats.high_watermark = data;
        }
        space--;

        stats.write_count++;
    }

    void decrement_data()
    {
        data--;
        space++;

        stats.read_count++;
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
