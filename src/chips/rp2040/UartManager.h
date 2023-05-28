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
bool getc_nonblocking(uint8_t &data);

template <size_t tx_depth, size_t rx_depth> class UartManager
{
    using TxBuffer = PcBuffer<tx_depth>;
    using RxBuffer = PcBuffer<rx_depth>;

  public:
    UartManager(uart_inst_t *_uart, bool _echo = true,
                bool _add_carriage_return = true)
        : tx_buffer(), rx_buffer(), uart(_uart), echo(_echo),
          add_carriage_return(_add_carriage_return)
    {
        /*
         * When data becomes available in the transmit buffer, service the TX
         * end (in case the UART has TX FIFO space).
         */
        tx_buffer.set_data_available([this](TxBuffer *buf) {
            (void)buf;
            service_tx();
        });

        /*
         * When space in the receive buffer is available, attempt to popupate
         * it with new data if the UART has received any.
         */
        rx_buffer.set_space_available([this](RxBuffer *buf) {
            (void)buf;
            service_rx();
        });
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

        tx_buffer.push_blocking(data);
    }

    void flush(void)
    {
        tx_buffer.flush();

        /* Wait for UART to be completely empty. */
        uart_tx_wait_blocking(uart);
    }

    bool getc_nonblocking(uint8_t &data)
    {
        /* Use this as an opportunity to service the receive buffer. */
        service_rx();
        return rx_buffer.pop(data);
    }

  protected:
    TxBuffer tx_buffer;
    RxBuffer rx_buffer;
    uart_inst_t *uart;
    bool echo;
    bool add_carriage_return;

    void service_tx(void)
    {
        uint8_t data;
        while (uart_is_writable(uart) and !tx_buffer.empty())
        {
            assert(tx_buffer.pop(data));
            uart_get_hw(uart)->dr = data;
        }
    }

    void service_rx(void)
    {
        uint8_t data;
        while (uart_is_readable(uart) and !rx_buffer.full())
        {
            data = uart_getc(uart);
            assert(rx_buffer.push(data));

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
