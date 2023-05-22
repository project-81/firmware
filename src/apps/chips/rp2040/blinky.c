/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/common.h"

/* third-party */
#include "hardware/gpio.h"
#include "hardware/regs/clocks.h"
#include "hardware/uart.h"
#include "pico/time.h"

static inline void gpio_toggle(uint gpio)
{
    gpio_xor_mask(1u << gpio);
}

void app(uint led_pin)
{
    dump_clocks();

    unsigned int iterations = 0;
    bool app = true;

    while (app)
    {
        gpio_toggle(led_pin);

        sleep_until(delayed_by_us(get_absolute_time(), 100 * 1000));

        if (uart_is_readable(uart0))
        {
            char result = uart_getc(uart0);
            switch (result)
            {
            case 113: /* 'q' */
                printf("Rebooting.\r\n");
                reset_bootloader();
                break;

            case 114: /* 'r' */
                printf("Restarting app.\r\n");
                app = false;
                break;

            default:
                printf("got: '%c' (%d)\r\n", result, result);
            }
        }

        if (iterations % 20 == 0)
        {
            printf("Hello, world! (%d)\r\n", iterations);
        }
        iterations++;
    }
}

void init(uint led_pin)
{
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    /* Initialize UART pins and peripheral. */
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_init(uart0, 115200);
}

int main(void)
{
    /* Default LED pin. */
    const uint led_pin = 25;
    init(led_pin);

    while (true)
    {
        app(led_pin);
    }
}
