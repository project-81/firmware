/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/common.h"
#include "generated/chips/rp2040/ws2812.h"

/* third-party */
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/regs/clocks.h"
#include "hardware/structs/scb.h"
#include "hardware/uart.h"
#include "pico/time.h"

static inline void gpio_toggle(uint gpio)
{
    gpio_xor_mask(1u << gpio);
}

bool poll_input(void)
{
    bool result = true;

    if (uart_is_readable(uart0))
    {
        char result = uart_getc(uart0);
        switch (result)
        {
        case 'q':
            printf("Resetting to bootloader.\r\n");
            reset(true);

        case 'r':
            printf("Restarting app.\r\n");
            result = false;
            break;

        case 'R':
            printf("Resetting to app.\r\n");
            reset(false);

        default:
            printf("got: '%c' (%d)\r\n", result, result);
        }
    }

    return result;
}

const PIO pio = pio0;
const uint sm = 0;

void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void app(uint led_pin)
{
    dump_clocks();

    unsigned int iterations = 0;
    bool app = true;

    uint8_t color_val = 0;

    while (app)
    {
        /* LED heartbeat. */
        gpio_toggle(led_pin);

        sleep_until(delayed_by_us(get_absolute_time(), 100 * 1000));

        app = poll_input();

        for (uint8_t i = 0; i < 2; i++)
        {
            put_pixel(urgb_u32(color_val, 0, 0));
            put_pixel(urgb_u32(0, color_val, 0));
            put_pixel(urgb_u32(0, 0, color_val));
        }

        color_val += 10;

        /* UART heartbeat. */
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

    /* Run a WS2812 LED driver in a PIO state machine. */
    ws2812_program_init(pio, sm, 18, false);

    printf("\r\nInitialization comlete.\r\n");
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
