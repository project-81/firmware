#include <stdio.h>

#include "pico/bootrom.h"
#include "pico/stdlib.h"

void reset_bootloader()
{
    rom_reset_usb_boot_fn func =
        (rom_reset_usb_boot_fn)rom_func_lookup(ROM_FUNC_RESET_USB_BOOT);
    func(0, 0);
}

int main()
{
    /* Default LED pin. */
    const uint led_pin = 25;

    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    /* Default settings. */
    stdio_uart_init_full(uart0, 115200, 0, 1);

    printf("Program start.\r\n");

    int iterations = 0;
    while (true)
    {
        gpio_put(led_pin, 1);
        sleep_ms(50);
        gpio_put(led_pin, 0);
        sleep_ms(50);

        int result = getchar_timeout_us(0);
        if (result >= 0)
        {
            printf("got: '%c' (%d)\r\n", result, result);
            if (result == 113) /* 'q' */
            {
                printf("Rebooting.\r\n");
                reset_bootloader();
            }
        }

        printf("Hello, world! (%d)\r\n", iterations);
        iterations++;
    }
}
