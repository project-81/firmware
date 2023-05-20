#include <stdio.h>

#include "pico/stdlib.h"

int main()
{
    /* Default LED pin. */
    const uint led_pin = 25;

    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    /* Default settings. */
    stdio_uart_init_full(uart0, 115200, 0, 1);

    while (true)
    {
        gpio_put(led_pin, 1);
        sleep_ms(100);
        gpio_put(led_pin, 0);
        sleep_ms(100);

        int result = getchar_timeout_us(0);
        if (result >= 0)
        {
            printf("%c", result);
        }

        printf("Hello, world!\r\n");
    }
}
