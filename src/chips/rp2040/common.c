/* toolchain */
#include <stdio.h>

/* internal */
#include "chips/rp2040/common.h"

/* third-party */
#include "hardware/clocks.h"
#include "pico/bootrom.h"

void reset_bootloader()
{
    rom_reset_usb_boot_fn func =
        (rom_reset_usb_boot_fn)rom_func_lookup(ROM_FUNC_RESET_USB_BOOT);
    func(0, 0);
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
