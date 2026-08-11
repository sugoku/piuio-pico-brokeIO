/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#include "lights_latch32.h"

#include "bsp/board.h"

PIO pio;
uint sm;

#define C_SER 11
#define C_SER2 4
#define C_RCLK 3
#define C_SRCLK 2

// Pad lamp bits this board actually drives, in the order the staggered update
// walks them. Every other bit in the caller's 32-bit lights word (cabinet
// marquee, neon, mux select) has no hardware behind it on this build.
static const uint8_t lamp_bits[] = {
    LATCH_P1L_DOWNRIGHT, LATCH_P1L_UPRIGHT, LATCH_P1L_CENTER,
    LATCH_P1L_DOWNLEFT,  LATCH_P1L_UPLEFT,
    LATCH_P2L_DOWNRIGHT, LATCH_P2L_UPRIGHT, LATCH_P2L_CENTER,
    LATCH_P2L_DOWNLEFT,  LATCH_P2L_UPLEFT,
};
#define LAMP_COUNT ((uint8_t)(sizeof(lamp_bits) / sizeof(lamp_bits[0])))

// what is currently shifted into the 74HC595s
static uint32_t latched = 0;
// round-robin cursor, so a lamp toggling every frame cannot starve the others
static uint8_t next_lamp = 0;
// loops remaining before another output is allowed to switch
static uint8_t change_delay = 0;
// when the state was last shifted out, for the idle refresh
static uint32_t last_shift_ts = 0;

static void lights_shift_out(uint32_t buf);

void lights_init() {

    gpio_init(C_SER);
    gpio_init(C_SER2);
    gpio_init(C_RCLK);
    gpio_init(C_SRCLK);

    // set pinModes for non-native SPI pins
    gpio_set_dir(C_SER, 1);
    gpio_set_dir(C_SER2, 1);
    gpio_set_dir(C_RCLK, 1);
    gpio_set_dir(C_SRCLK, 1);

    gpio_put(C_SER, 0);
    gpio_put(C_SER2, 0);
    gpio_put(C_RCLK, 1);
    gpio_put(C_SRCLK, 1);

    // the 595 outputs are undefined at power-up, so shift out a known all-off
    // frame before the staggered updates start tracking `latched`
    latched = 0;
    next_lamp = 0;
    change_delay = 0;
    lights_shift_out(latched);
    last_shift_ts = board_millis();
}

void lights_shift() {
    gpio_put(C_SRCLK, 0);
    busy_wait_us(5);
    gpio_put(C_SRCLK, 1);
    busy_wait_us(5);
}

/** Shift the ten pad lamp bits into both 74HC595s and pulse RCLK to latch them. */
static void lights_shift_out(uint32_t buf) {
    // tell latch to receive from SPI
    gpio_put(C_RCLK, 0);
    // the RP2040 is going too fast for our 74HC595s, so we need to delay every time we send messages out
    busy_wait_us(5);
    // note that we are using busy_wait_us instead of sleep_us because sleep_us halts our USB activity


    gpio_put(C_SER, GETBIT(buf, LATCH_P1L_DOWNRIGHT));
    gpio_put(C_SER2, GETBIT(buf, LATCH_P2L_DOWNRIGHT));
    lights_shift();

    gpio_put(C_SER, GETBIT(buf, LATCH_P1L_UPRIGHT));
    gpio_put(C_SER2, GETBIT(buf, LATCH_P2L_UPRIGHT));
    lights_shift();

    gpio_put(C_SER, GETBIT(buf, LATCH_P1L_CENTER));
    gpio_put(C_SER2, GETBIT(buf, LATCH_P2L_CENTER));
    lights_shift();

    gpio_put(C_SER, GETBIT(buf, LATCH_P1L_DOWNLEFT));
    gpio_put(C_SER2, GETBIT(buf, LATCH_P2L_DOWNLEFT));
    lights_shift();

    gpio_put(C_SER, GETBIT(buf, LATCH_P1L_UPLEFT));
    gpio_put(C_SER2, GETBIT(buf, LATCH_P2L_UPLEFT));
    lights_shift();


    gpio_put(C_SER, 0);
    gpio_put(C_SER2, 0);
    lights_shift();
/*
#define LATCH_P1L_UPLEFT 29
#define LATCH_P1L_UPRIGHT 28
#define LATCH_P1L_CENTER 27
#define LATCH_P1L_DOWNLEFT 26
#define LATCH_P1L_DOWNRIGHT 25

#define LATCH_P2L_UPLEFT 13
#define LATCH_P2L_UPRIGHT 12
#define LATCH_P2L_CENTER 11
#define LATCH_P2L_DOWNLEFT 10
#define LATCH_P2L_DOWNRIGHT 9
*/

    // // worst case 10-15ns needed according to datasheet, but i guess we need longer
    // #ifndef SOFTWARE_LATCH
    //     spi_write_blocking(spi1, buf, 4);
    // #else
    //     sspi_out_put(pio, sm, (uint8_t)((*buf & 0xFF000000) >> 24));
    //     busy_wait_us(1);
    //     sspi_out_put(pio, sm, (uint8_t)((*buf & 0x00FF0000) >> 16));
    //     busy_wait_us(1);
    //     sspi_out_put(pio, sm, (uint8_t)((*buf & 0x0000FF00) >> 8));
    //     busy_wait_us(1);
    //     sspi_out_put(pio, sm, (uint8_t)((*buf & 0x000000FF)));
    // #endif

    busy_wait_us(5);

    // tell latch to update values with what we just sent
    gpio_put(C_RCLK, 1);

    busy_wait_us(5);
}

/**
 * Switch at most one lamp output every LIGHTS_CHANGE_INTERVAL calls, and
 * refresh the unchanged state every LIGHTS_REFRESH_MS.
 *
 * The 74HC595s latch all ten outputs on a single RCLK edge, so a naive update
 * can switch every lamp at once - ten simultaneous inrush currents sharing one
 * 12V ground return. Rationing changes spreads that into separate events, at
 * the cost of about 16ms for a full ten lamp change.
 *
 * The periodic refresh re-sends what is already latched, so no output switches
 * and no inrush follows, but a bit corrupted inside a 595 by a noise event gets
 * repaired within LIGHTS_REFRESH_MS instead of sticking until the next change.
 */
void lights_send(uint32_t* buf) {
    uint32_t now = board_millis();

    if (change_delay > 0) {
        change_delay--;
    } else {
        uint8_t change = LAMP_COUNT;

        for (uint8_t i = 0; i < LAMP_COUNT; i++) {
            uint8_t idx = (next_lamp + i) % LAMP_COUNT;
            uint8_t bit = lamp_bits[idx];

            if (!GETBIT(*buf, bit) != !GETBIT(latched, bit)) {
                change = idx;
                break;
            }
        }

        if (change != LAMP_COUNT) {
            SETORCLRBIT(latched, lamp_bits[change], GETBIT(*buf, lamp_bits[change]));
            next_lamp = (change + 1) % LAMP_COUNT;
            change_delay = LIGHTS_CHANGE_INTERVAL - 1;

            lights_shift_out(latched);
            last_shift_ts = now;
            return;
        }
    }

    if (now - last_shift_ts >= LIGHTS_REFRESH_MS) {
        lights_shift_out(latched);
        last_shift_ts = now;
    }
}
