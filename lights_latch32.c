/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#include "lights_latch32.h"

PIO pio;
uint sm;

#define C_SER 11
#define C_SER2 4
#define C_RCLK 3
#define C_SRCLK 2

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
}

void lights_shift() {
    gpio_put(C_SRCLK, 0);
    busy_wait_us(5);
    gpio_put(C_SRCLK, 1);
    busy_wait_us(5);
}

void lights_send(uint32_t* buf) {
    // tell latch to receive from SPI
    gpio_put(C_RCLK, 0);
    // the RP2040 is going too fast for our 74HC595s, so we need to delay every time we send messages out
    busy_wait_us(5);
    // note that we are using busy_wait_us instead of sleep_us because sleep_us halts our USB activity


    gpio_put(C_SER, GETBIT(*buf, LATCH_P1L_DOWNRIGHT));
    gpio_put(C_SER2, GETBIT(*buf, LATCH_P2L_DOWNRIGHT));
    lights_shift();
    
    gpio_put(C_SER, GETBIT(*buf, LATCH_P1L_UPRIGHT));
    gpio_put(C_SER2, GETBIT(*buf, LATCH_P2L_UPRIGHT));
    lights_shift();

    gpio_put(C_SER, GETBIT(*buf, LATCH_P1L_CENTER));
    gpio_put(C_SER2, GETBIT(*buf, LATCH_P2L_CENTER));
    lights_shift();

    gpio_put(C_SER, GETBIT(*buf, LATCH_P1L_DOWNLEFT));
    gpio_put(C_SER2, GETBIT(*buf, LATCH_P2L_DOWNLEFT));
    lights_shift();

    gpio_put(C_SER, GETBIT(*buf, LATCH_P1L_UPLEFT));
    gpio_put(C_SER2, GETBIT(*buf, LATCH_P2L_UPLEFT));
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
