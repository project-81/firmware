#pragma once

/* toolchain */
#include <cstdint>

namespace Project81
{

class PcBufferState
{
  public:
    PcBufferState(std::size_t _size)
        : size(_size), data(0), space(_size), high_watermark(0),
          write_dropped(0)
    {
    }

    void reset(void)
    {
        /* Reset state. */
        data = 0;
        space = size;

        /* Reset stats. */
        high_watermark = 0;
        write_dropped = 0;
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
            if (data > high_watermark)
            {
                high_watermark = data;
            }
            space -= count;
        }
        else if (drop)
        {
            write_dropped += count;
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
        }

        return result;
    }

    void poll_metrics(uint16_t &_high_watermark, uint16_t &_write_dropped,
                      bool reset = true)
    {
        _high_watermark = high_watermark;
        _write_dropped = write_dropped;
        if (reset)
        {
            high_watermark = 0;
            write_dropped = 0;
        }
    }

    inline bool empty(void)
    {
        return data == 0;
    }

    inline bool full(void)
    {
        return space == 0;
    }

    inline std::size_t data_available(void)
    {
        return data;
    }

    inline std::size_t space_available(void)
    {
        return data;
    }

    const std::size_t size;

  protected:
    std::size_t data;
    std::size_t space;

    uint16_t high_watermark;
    uint16_t write_dropped;
};

} // namespace Project81
