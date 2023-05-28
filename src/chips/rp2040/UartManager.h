#pragma once

/* toolchain */
#include <cassert>

/* third-party */
#include "hardware/uart.h"

/* internal */
#include "common/buffer/FullDuplexBuffer.h"
#include "common/buffer/PcBuffer.h"

namespace Project81
{

template <size_t tx_depth, size_t rx_depth>
class UartManager : public FullDuplexBuffer<tx_depth, rx_depth>
{
    using FdBuffer = FullDuplexBuffer<tx_depth, rx_depth>;

  public:
    UartManager(uart_inst_t *_uart, bool _echo = true,
                bool _add_carriage_return = true)
        : FdBuffer(), uart(_uart), echo(_echo),
          add_carriage_return(_add_carriage_return)
    {
    }

    void putc_block(uint8_t data)
    {
        switch (data)
        {
        case '\n':
            /* If we should add carriage returns, add it first. */
            if (add_carriage_return)
            {
                putc_block('\r');
            }
            break;
        }

        this->tx_buffer.push_blocking(data);
    }

    void flush(void)
    {
        this->tx_buffer.flush();

        /* Wait for UART to be completely empty. */
        uart_tx_wait_blocking(uart);
    }

    bool getc_nonblocking(uint8_t &data)
    {
        /* Use this as an opportunity to service the receive buffer. */
        service_rx(&this->rx_buffer);
        return this->rx_buffer.pop(data);
    }

  protected:
    uart_inst_t *uart;
    bool echo;
    bool add_carriage_return;

    void service_tx(FdBuffer::TxBuffer *buf)
    {
        uint8_t data;
        while (uart_is_writable(uart) and !buf->empty())
        {
            assert(buf->pop(data));
            uart_get_hw(uart)->dr = data;
        }
    }

    void service_rx(FdBuffer::RxBuffer *buf)
    {
        uint8_t data;
        while (uart_is_readable(uart) and !buf->full())
        {
            data = uart_getc(uart);
            assert(buf->push(data));

            if (echo)
            {
                switch (data)
                {
                case '\r':
                    putc_block('\n');

                    /*
                     * If carriage returns are getting added automatically,
                     * the '\n' put will already add one.
                     */
                    if (add_carriage_return)
                    {
                        break;
                    }

                    [[fallthrough]];
                default:
                    putc_block(data);
                }
            }
        }
    }
};

}; // namespace Project81
