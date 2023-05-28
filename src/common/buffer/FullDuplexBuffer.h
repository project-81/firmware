#pragma once

/* internal */
#include "common/buffer/PcBuffer.h"

namespace Project81
{

template <size_t tx_depth, size_t rx_depth> class FullDuplexBuffer
{
  public:
    using TxBuffer = PcBuffer<tx_depth>;
    using RxBuffer = PcBuffer<rx_depth>;

    FullDuplexBuffer() : tx_buffer(), rx_buffer()
    {
        /*
         * Attempt to service the writing end whenever data is ready to be
         * written.
         */
        tx_buffer.set_data_available(
            [this](TxBuffer *buf) { service_tx(buf); });

        /*
         * Attempt to service the reading end whenever the read buffer has
         * space.
         */
        rx_buffer.set_space_available(
            [this](RxBuffer *buf) { service_rx(buf); });
    }

    /*
     * A method that can be polled at runtime if it's useful for hardware
     * resources to be interacted with regularly.
     */
    void dispatch(void)
    {
        service_tx(&tx_buffer);
        service_rx(&rx_buffer);
    }

  protected:
    TxBuffer tx_buffer;
    RxBuffer rx_buffer;

    /* Enables blocking interface-semantics on underlying buffers. */
    virtual void service_tx(TxBuffer *buf) = 0;
    virtual void service_rx(RxBuffer *buf) = 0;
};

} // namespace Project81
