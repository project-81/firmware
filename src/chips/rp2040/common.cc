/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/common.h"

/* third-party */
#include "hardware/clocks.h"
#include "hardware/structs/scb.h"
#include "pico/bootrom.h"

namespace Project81
{
void reset(bool bootloader)
{
    fflush(stdout);

    if (bootloader)
    {
        rom_reset_usb_boot_fn func =
            (rom_reset_usb_boot_fn)rom_func_lookup(ROM_FUNC_RESET_USB_BOOT);
        func(0, 0);
    }
    else
    {
        /* Perform a reset via the Application Interrupt and Reset Control
         * Register. */
        scb_hw->aircr = (0x05FA << M0PLUS_AIRCR_VECTKEY_LSB) |
                        M0PLUS_AIRCR_SYSRESETREQ_BITS;
    }

    tight_loop_contents();
    __builtin_unreachable();
}

void dump_clocks()
{
    printf("pll_sys  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY));
    printf("pll_usb  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY));
    printf("rosc     = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC));
    printf("clk_sys  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS));
    printf("clk_peri = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI));
    printf("clk_usb  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB));
    printf("clk_adc  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC));
    printf("clk_rtc  = %lu kHz\r\n",
           frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC));
}
}; // namespace Project81
