/* toolchain */
#include <cassert>
#include <limits>
#include <stdio.h>

/* internal */
#include "common/buffer/PcBuffer.h"

static constexpr std::size_t depth = 1024;
using element_t = std::uint8_t;

using namespace Project81;

using Buffer = PcBuffer<depth, element_t>;

void test_basic(Buffer &buf)
{
    buf.clear();

    element_t val = 0;
    for (std::size_t i = 0; i < depth; i++)
    {
        /* Should be able to fill the buffer. */
        assert(buf.push(val));
        val++;
    }

    PcBufferStats stats;
    buf.state.poll_stats(stats);
    assert(stats.high_watermark == depth);

    /* Should not be able to add any more data. */
    assert(!buf.push(val));

    for (std::size_t i = 0; i < depth; i++)
    {
        assert(buf.pop(val));
        assert(val == (i % (std::numeric_limits<element_t>::max() + 1)));
    }

    /* Should not be able to read any more data. */
    assert(!buf.pop(val));
}

void test_n_push_pop(Buffer &buf)
{
    buf.clear();

    element_t tmp = 0;
    element_t data[depth];
    for (std::size_t i = 0; i < depth; i++)
    {
        data[i] = tmp++;
    }

    assert(buf.push_n(data, depth));

    element_t new_data[depth];
    assert(buf.pop_n(new_data, depth));

    /* Verify contents. */
    for (std::size_t i = 0; i < depth; i++)
    {
        assert(data[i] == new_data[i]);
    }

    assert(buf.pop_all(new_data) == 0);
    assert(buf.push_n(data, depth));
    assert(buf.pop_all(new_data) == depth);
}

int main(void)
{
    Buffer buf;
    assert(buf.empty());
    assert(!buf.full());

    test_basic(buf);
    test_n_push_pop(buf);

    return 0;
}
