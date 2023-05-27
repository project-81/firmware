#pragma once

/* toolchain */
#include <cstdint>

namespace Project81
{

struct PcBufferStats
{
    uint16_t high_watermark = 0;
    uint32_t read_count = 0;
    uint32_t write_count = 0;

    uint16_t write_dropped = 0;

    void reset()
    {
        high_watermark = 0;
        read_count = 0;
        write_count = 0;
        write_dropped = 0;
    }
};

}; // namespace Project81
