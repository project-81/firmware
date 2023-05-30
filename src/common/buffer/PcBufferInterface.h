#pragma once

/* toolchain */
#include <cstdint>

namespace Project81
{

template <typename element_t = uint8_t> class PcBufferWriter
{
  public:
    virtual bool push(const element_t elem, bool drop = false) = 0;

    virtual void push_blocking(const element_t elem) = 0;

    virtual bool push_n(const element_t *elem_array, std::size_t count,
                        bool drop = false) = 0;

    virtual std::size_t try_push_n(const element_t *elem_array,
                                   std::size_t count) = 0;

    virtual void push_n_blocking(const element_t *elem_array,
                                 std::size_t count) = 0;
};

template <typename element_t = uint8_t> class PcBufferReader
{
  public:
    virtual bool pop(element_t &elem) = 0;

    virtual bool pop_n(element_t *elem_array, std::size_t count) = 0;

    virtual std::size_t try_pop_n(element_t *elem_array,
                                  std::size_t count) = 0;

    virtual std::size_t pop_all(element_t *elem_array = nullptr) = 0;
};

}; // namespace Project81
