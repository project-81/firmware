/* internal */
#include "generated/chips/rp2040/ws2812.h"

/* third-party */
#include "hardware/clocks.h"

static uint ws2812_offset;
static bool ws2812_added = false;

const float ws2812_frequency = 800000.0f;

void ws2812_program_init(PIO pio, uint sm, uint pin, bool rgbw)
{
    /*
     * Add the program just-in-time. Instead we should make sure this is
     * never done twice.
     */
    if (!ws2812_added)
    {
        ws2812_offset = pio_add_program(pio, &ws2812_program);
        ws2812_added = true;
    }

    /* Setup the output pin. */
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(ws2812_offset);

    /* Program implementation uses sideset. */
    sm_config_set_sideset_pins(&c, pin);

    /* Data word size depends on the kind of LED. */
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);

    /* Use entire FIFO for processor-to-state-machine. */
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    /* Set up the appropriate clock division. */
    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (ws2812_frequency * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    /* Finalize the state machine. */
    pio_sm_init(pio, sm, ws2812_offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
