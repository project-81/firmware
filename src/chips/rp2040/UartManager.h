#pragma once

/* toolchain */
#include <cassert>

/* third-party */
#include "hardware/uart.h"

/* internal */
#include "common/buffer/PcBuffer.h"

namespace Project81
{

/*
 * Since a UART inevitably plays a role in standard input/output,
 * forward-declare some functions for implementing the desired behavior.
 */
void service_stdio_uart(void);
bool getc_nonblocking(uint8_t &data);

template <size_t tx_depth, size_t rx_depth> class UartManager
{
  public:
    UartManager(uart_inst_t *_uart, bool _echo = true,
                bool _add_carriage_return = true)
        : tx_buffer(), rx_buffer(), uart(_uart), echo(_echo),
          add_carriage_return(_add_carriage_return)
    {
    }

    void dispatch(void)
    {
        /* Service TX direction. */
        uint8_t data;
        while (uart_is_writable(uart) and !tx_buffer.empty())
        {
            assert(tx_buffer.pop(data));
            uart_get_hw(uart)->dr = data;
        }

        /* Service RX direction. */
        while (uart_is_readable(uart) and !rx_buffer.full())
        {
            data = uart_getc(uart);
            assert(rx_buffer.push(data));

            if (echo)
            {
                switch (data)
                {
                case '\r':
                    putc_block_if_full('\n');

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
                    putc_block_if_full(data);
                }
            }
        }
    }

    void putc_block_if_full(uint8_t data)
    {
        switch (data)
        {
        case '\n':
            /* If we should add carriage returns, add it first. */
            if (add_carriage_return)
            {
                putc_block_if_full('\r');
            }
            break;
        }

        while (tx_buffer.full())
        {
            dispatch();
        }

        assert(tx_buffer.push(data));
    }

    void flush(void)
    {
        while (!tx_buffer.empty())
        {
            dispatch();
        }

        /* Wait for UART to be completely empty. */
        uart_tx_wait_blocking(uart);
    }

    bool getc_nonblocking(uint8_t &data)
    {
        return rx_buffer.pop(data);
    }

  protected:
    PcBuffer<tx_depth> tx_buffer;
    PcBuffer<rx_depth> rx_buffer;
    uart_inst_t *uart;
    bool echo;
    bool add_carriage_return;
};

}; // namespace Project81
