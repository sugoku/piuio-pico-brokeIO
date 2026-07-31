/*******************************************************************************************/
/*  SPDX-License-Identifier: MIT                                                           */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 48productions, therathatter, dj505, sugoku  */
/*  https://github.com/sugoku/piuio-pico-brokeIO                                           */
/*******************************************************************************************/

#ifndef _PIUIO_CONFIG_H
#define _PIUIO_CONFIG_H
#include "piuio_ws2812_helpers.h"

#include "hardware/timer.h"

#include "usb_descriptors.h"
#include "usb_hid_keys.h"

// debounce time in milliseconds
// adjust if you are getting misfires!
#define DEBOUNCE_PRESS_TIME 15
#define DEBOUNCE_RELEASE_TIME 15

// enable debouncing
#define DEBOUNCING

// use joystick instead of d-pad in Switch mode
#define SWITCH_JOYSTICK

// merge all sensor inputs, improving response time at the cost of not being able
// to read individual sensors in the service menu.
// in PIUIO mode, one poll reads one sensor per panel (10/40 sensors per poll).
// official games only poll the PIUIO every 10ms, which is 100Hz best case but
// 25Hz worst case the game is reading the wrong sensor.
// setting this to true will improve sensor press polling to always be 100Hz,
// but release polling will still be 25Hz worst case.
#define MERGE_MUX_PIUIO true

// the LXIO (at least the older versions) has an incorrect USB descriptor
// leaving this uncommented will fix andamiro's mistake
// #define LXIO_FIX_DESCRIPTOR

// always send an LXIO report even when nothing has changed
// although this is less efficient this what the real I/O does
#define LXIO_ALWAYS_SEND_REPORT true

// time in microseconds to wait between input/lights operations
// this accounts for variance in the brokeIO's multiplexer and latch chips
// the delay is negligible, so you would not notice it
#define WAIT_INPUT_MUX4067 20
#define WAIT_LIGHTS_LATCH32 20

// always allow pad combo to enter bootloader; otherwise, it must be done in the service mode
#define ALWAYS_BOOTLOADER false

// default input mode unless otherwise specified in the flash memory
#define DEFAULT_INPUT_MODE INPUT_MODE_SERIAL

// use software SPI to control latch for outputs
// for some reason hardware SPI wasn't working right for me so I have it enabled
#define SOFTWARE_LATCH

// toggle pin on and off on the main loop for debugging purposes
// #define BENCHMARK

// uncomment to always use the default input mode on boot instead of what's in the flash memory
// disables reading/writing to flash also
// (you will not be able to change the mode until reflashing!)
// #define ALWAYS_DEFAULT_INPUT_MODE

// threshold in ms to hold SERVICE button to enter mode select (settings menu)
#define SETTINGS_THRESHOLD 2000

// these PIDs are granted by Openmoko! they are not used for PIUIO/LXIO modes
// https://github.com/openmoko/openmoko-usb-oui
#define VENDOR_ID               0x1D50
#define PRODUCT_ID_GAMEPAD      0x6181
#define PRODUCT_ID_KEYBOARD     0x6182
#define PRODUCT_ID_OTHER        0x6183

// enable pullup resistors for inputs
// (only disable this if you know what you are doing!)
#define PULLUP_IN

#define MUX_GLOBAL 4
#define MUX_COUNT 5

#define MAX_USB_POWER 0xFA  // (500mA)

// Uncomment these defines to enable WS2812 LED support. NOT WORKING WITH brokeIO
//#define ENABLE_WS2812_SUPPORT
//#define WS2812_IS_RGBW false
//#define WS2812_PIN 22

// Modify these to edit the colors of the cabinet lamps.
#ifdef ENABLE_WS2812_SUPPORT
static uint32_t ws2812_color[5] = {
        urgb_u32(0, 255, 0),    // Lower left
        urgb_u32(255, 0, 0),    // Upper left
        urgb_u32(0, 0, 255),    // Bass / neon
        urgb_u32(255, 0, 0),    // Upper right
        urgb_u32(0, 255, 0)     // Lower right
};
#endif

// helper defines

#define GETBIT(port,bit) ((port) & (1 << (bit)))     // get value at bit
#define SETBIT(port,bit) ((port) |= (1 << (bit)))    // set bit to 1
#define CLRBIT(port,bit) ((port) &= ~(1 << (bit)))   // set bit to 0 (clear bit)
#define SETORCLRBIT(port,bit,val) if (val) { SETBIT(port,bit); } else { CLRBIT(port,bit); }  // if true, set bit to 1, if false, clear bit to 0

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)


// prototype boards from july to december 2023 have a different pinout (blue, forward facing molex)
//#define IDC_FLIP_REV

#if defined(IDC_FLIP_REV)
// Modify these arrays to edit the pin out.
// Map these according to your button pins.
#define MUX4067_P1_UPLEFT 7
#define MUX4067_P1_UPRIGHT 6
#define MUX4067_P1_CENTER 5
#define MUX4067_P1_DOWNLEFT 4
#define MUX4067_P1_DOWNRIGHT 3
// MUX4067_P1_MENU_SELECT would be same as JAMMA_W
// MUX4067_P1_MENU_L would be same as JAMMA_X
// MUX4067_P1_MENU_R would be same as JAMMA_Y

#define MUX4067_P2_SELECT 23
#define MUX4067_P2_MENU_L 22
#define MUX4067_P2_MENU_R 21
#define MUX4067_P2_UPLEFT 20
#define MUX4067_P2_UPRIGHT 19
#define MUX4067_P2_CENTER 18
#define MUX4067_P2_DOWNLEFT 17
#define MUX4067_P2_DOWNRIGHT 16

#define MUX4067_P1_COIN 10
#define MUX4067_P2_COIN 29

#define MUX4067_TEST 9
#define MUX4067_SERVICE 14
#define MUX4067_CLEAR 15

#define MUX4067_JAMMA_Q 11
#define MUX4067_JAMMA_17 28
#define MUX4067_JAMMA_W 2
#define MUX4067_JAMMA_X 1
#define MUX4067_JAMMA_Y 0
#define MUX4067_JAMMA_Z 8

// Map these according to your LED pins.
#define LATCH_P1L_UPLEFT 2
#define LATCH_P1L_UPRIGHT 3
#define LATCH_P1L_CENTER 4
#define LATCH_P1L_DOWNLEFT 5
#define LATCH_P1L_DOWNRIGHT 6

#define LATCH_P2L_UPLEFT 18
#define LATCH_P2L_UPRIGHT 19
#define LATCH_P2L_CENTER 20
#define LATCH_P2L_DOWNLEFT 21
#define LATCH_P2L_DOWNRIGHT 22

#define LATCH_P1_S0 0
#define LATCH_P1_S1 1
#define LATCH_P2_S0 16
#define LATCH_P2_S1 17

#define LATCH_CABL_MARQ1 25
#define LATCH_CABL_MARQ2 23
#define LATCH_CABL_MARQ3 24
#define LATCH_CABL_MARQ4 26
#define LATCH_CABL_NEON 10

#define LATCH_COIN_COUNTER 28
#define LATCH_ALWAYS_ON 27

#define LATCH_JAMMA_LED 14
#else
// Modify these arrays to edit the pin out.
// Map these according to your button pins.
#define MUX4067_P1_UPLEFT 23
#define MUX4067_P1_UPRIGHT 22
#define MUX4067_P1_CENTER 21
#define MUX4067_P1_DOWNLEFT 20
#define MUX4067_P1_DOWNRIGHT 19
// MUX4067_P1_MENU_SELECT would be same as JAMMA_W
// MUX4067_P1_MENU_L would be same as JAMMA_X
// MUX4067_P1_MENU_R would be same as JAMMA_Y

#define MUX4067_P2_SELECT 7
#define MUX4067_P2_MENU_L 6
#define MUX4067_P2_MENU_R 5
#define MUX4067_P2_UPLEFT 4
#define MUX4067_P2_UPRIGHT 3
#define MUX4067_P2_CENTER 2
#define MUX4067_P2_DOWNLEFT 1
#define MUX4067_P2_DOWNRIGHT 0

#define MUX4067_P1_COIN 29
#define MUX4067_P2_COIN 10

#define MUX4067_TEST 30
#define MUX4067_SERVICE 25
#define MUX4067_CLEAR 24

#define MUX4067_JAMMA_Q 28
#define MUX4067_JAMMA_17 11
#define MUX4067_JAMMA_W 18
#define MUX4067_JAMMA_X 17
#define MUX4067_JAMMA_Y 16
#define MUX4067_JAMMA_Z 31

// Map these according to your LED pins.
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

#define LATCH_P1_S0 31
#define LATCH_P1_S1 30
#define LATCH_P2_S0 15
#define LATCH_P2_S1 14

#define LATCH_CABL_MARQ1 6
#define LATCH_CABL_MARQ2 8
#define LATCH_CABL_MARQ3 7
#define LATCH_CABL_MARQ4 5
#define LATCH_CABL_NEON 21

#define LATCH_COIN_COUNTER 3
#define LATCH_ALWAYS_ON 4

#define LATCH_JAMMA_LED 20

#endif

// other pins
#define MUX_ENABLE_PIN 21
#define MUX1_IN_PIN 26
#define MUX2_IN_PIN 27

#define MUX_S0_PIN 22
#define MUX_S1_PIN 23
#define MUX_S2_PIN 24
#define MUX_S3_PIN 25

#define LATCH_ENABLE_PIN 20
#define LATCH_RST_PIN 19
#define LATCH_RCLK_PIN 18

#define SOFTWARE_SPI_DIN_PIN 9
#define SOFTWARE_SPI_CLK_PIN 8

#define UART_TX_PIN 16
#define UART_RX_PIN 17
#define UART_SHDN_PIN 14
#define UART_RE_PIN 15

#define BENCHMARK_PIN_1 4
#define BENCHMARK_PIN_2 5
#define BENCHMARK_PIN_3 6
#define BENCHMARK_PIN_4 7


// other defines
// offset from XIP_BASE, let's make it 1MiB from the start
#define INPUT_MODE_OFFSET (1024 * 1024)
// turn LED on for 200ms every 400ms 
#define SERVICE_BLINK_LENGTH 200
#define SERVICE_BLINK_INTERVAL 400


// UART defines
#define UART_HOST true
#define UART_HOST_ID '0'
#define UART_DEVICE_ID '1'


// HID defines

// in lane order: zqsec 17593

#define KEYCODE_P1_UPLEFT KEY_Q
#define KEYCODE_P1_UPRIGHT KEY_E
#define KEYCODE_P1_CENTER KEY_S
#define KEYCODE_P1_DOWNLEFT KEY_Z
#define KEYCODE_P1_DOWNRIGHT KEY_C
#define KEYCODE_P1_MENU_SELECT KEY_X
#define KEYCODE_P1_MENU_LEFT KEY_A
#define KEYCODE_P1_MENU_RIGHT KEY_D

#define KEYCODE_P2_UPLEFT KEY_KP7
#define KEYCODE_P2_UPRIGHT KEY_KP9
#define KEYCODE_P2_CENTER KEY_KP5
#define KEYCODE_P2_DOWNLEFT KEY_KP1
#define KEYCODE_P2_DOWNRIGHT KEY_KP3
#define KEYCODE_P2_MENU_SELECT KEY_KP2
#define KEYCODE_P2_MENU_LEFT KEY_KP4
#define KEYCODE_P2_MENU_RIGHT KEY_KP6

// F5, F6
#define KEYCODE_P1_COIN KEY_F5
#define KEYCODE_P2_COIN KEY_F6

// F2, F9, F1
#define KEYCODE_TEST KEY_F2
#define KEYCODE_SERVICE KEY_F9
#define KEYCODE_CLEAR KEY_F1


#endif //_PIUIO_CONFIG_H
