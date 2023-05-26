#pragma once

/* third-party */
#include "hardware/uart.h"

/* internal */
#include "common/buffer/PcBuffer.h"

namespace Project81
{

template <std::size_t tx_depth, std::size_t rx_depth> class UartManager
{
  public:
    UartManager(uart_inst_t *_uart, bool _echo = true)
        : tx_buffer(), rx_buffer(), uart(_uart), echo(_echo)
    {
    }

    void dispatch(void)
    {
        /* Service TX direction. */
        uint8_t data;
        while (uart_is_writable(uart) and !tx_buffer.empty())
        {
            tx_buffer.pop(data);
            uart_get_hw(uart)->dr = data;
        }

        /* Service RX direction. */
        while (uart_is_readable(uart) and !rx_buffer.full())
        {
            data = uart_getc(uart);
            rx_buffer.push(data);

            if (echo)
            {
                putc_block_if_full(data);
            }
        }
    }

    void putc_block_if_full(std::size_t data)
    {
        while (tx_buffer.full())
        {
            dispatch();
        }

        tx_buffer.push(data);
    }

    void flush()
    {
        while (!tx_buffer.empty())
        {
            dispatch();
        }

        /* Wait for UART to be completely empty. */
        uart_tx_wait_blocking(uart);
    }

    bool getc_nonblocking(std::uint8_t &data)
    {
        return rx_buffer.pop(data);
    }

  protected:
    PcBuffer<tx_depth> tx_buffer;
    PcBuffer<rx_depth> rx_buffer;
    uart_inst_t *uart;
    bool echo;
};

void service_stdio_uart(void);

bool getc_nonblocking(std::uint8_t &data);

}; // namespace Project81
