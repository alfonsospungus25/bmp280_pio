// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --------- //
// spi_cpha0 //
// --------- //

#define spi_cpha0_wrap_target 0
#define spi_cpha0_wrap 1

static const uint16_t spi_cpha0_program_instructions[] = {
            //     .wrap_target
    0x6101, //  0: out    pins, 1         side 0 [1] 
    0x5101, //  1: in     pins, 1         side 1 [1] 
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program spi_cpha0_program = {
    .instructions = spi_cpha0_program_instructions,
    .length = 2,
    .origin = -1,
};

static inline pio_sm_config spi_cpha0_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + spi_cpha0_wrap_target, offset + spi_cpha0_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}
#endif

// --------- //
// spi_cpha1 //
// --------- //

#define spi_cpha1_wrap_target 0
#define spi_cpha1_wrap 2

static const uint16_t spi_cpha1_program_instructions[] = {
            //     .wrap_target
    0x6021, //  0: out    x, 1            side 0     
    0xb101, //  1: mov    pins, x         side 1 [1] 
    0x4001, //  2: in     pins, 1         side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program spi_cpha1_program = {
    .instructions = spi_cpha1_program_instructions,
    .length = 3,
    .origin = -1,
};

static inline pio_sm_config spi_cpha1_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + spi_cpha1_wrap_target, offset + spi_cpha1_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

#include "hardware/gpio.h"
static inline void pio_spi_init(PIO pio, uint sm, uint prog_offs, uint n_bits,
        float clkdiv, bool cpha, bool cpol, uint pin_sck, uint pin_mosi, uint pin_miso) {
    pio_sm_config c = cpha ? spi_cpha1_program_get_default_config(prog_offs) : spi_cpha0_program_get_default_config(prog_offs);
    sm_config_set_out_pins(&c, pin_mosi, 1);
    sm_config_set_in_pins(&c, pin_miso);
    sm_config_set_sideset_pins(&c, pin_sck);
    // Only support MSB-first in this example code (shift to left, auto push/pull, threshold=nbits)
    sm_config_set_out_shift(&c, false, true, n_bits);
    sm_config_set_in_shift(&c, false, true, n_bits);
    sm_config_set_clkdiv(&c, clkdiv);
    // MOSI, SCK output are low, MISO is input
    pio_sm_set_pins_with_mask(pio, sm, 0, (1u << pin_sck) | (1u << pin_mosi));
    pio_sm_set_pindirs_with_mask(pio, sm, (1u << pin_sck) | (1u << pin_mosi), (1u << pin_sck) | (1u << pin_mosi) | (1u << pin_miso));
    pio_gpio_init(pio, pin_mosi);
    pio_gpio_init(pio, pin_miso);
    pio_gpio_init(pio, pin_sck);
    // The pin muxes can be configured to invert the output (among other things
    // and this is a cheesy way to get CPOL=1
    gpio_set_outover(pin_sck, cpol ? GPIO_OVERRIDE_INVERT : GPIO_OVERRIDE_NORMAL);
    // SPI is synchronous, so bypass input synchroniser to reduce input delay.
    hw_set_bits(&pio->input_sync_bypass, 1u << pin_miso);
    pio_sm_init(pio, sm, prog_offs, &c);
    pio_sm_set_enabled(pio, sm, true);
}

#endif

