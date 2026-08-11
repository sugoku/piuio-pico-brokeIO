/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#include <string.h>

#include "input_mux4067.h"

#include "bsp/board.h"

uint32_t mux4067_vals[MUX_COUNT] = {0};
uint32_t mux4067_vals_db[MUX_COUNT] = {0};  // debounced

// debouncing helper arrays
uint32_t mux4067_vals_last[MUX_COUNT] = {0};
uint32_t press_ts[MUX_COUNT][32] = {0};
uint32_t release_ts[MUX_COUNT][32] = {0};

#define MUX_P1_INPUTS_SIZE 5
const uint8_t mux_p1_inputs[] = {
    MUX4067_P1_UPLEFT,
    MUX4067_P1_UPRIGHT,
    MUX4067_P1_CENTER,
    MUX4067_P1_DOWNLEFT,
    MUX4067_P1_DOWNRIGHT,
};
#define MUX_P2_INPUTS_SIZE 5
const uint8_t mux_p2_inputs[] = {
    MUX4067_P2_UPLEFT,
    MUX4067_P2_UPRIGHT,
    MUX4067_P2_CENTER,
    MUX4067_P2_DOWNLEFT,
    MUX4067_P2_DOWNRIGHT,
};

#define C_MUX11 9
#define C_MUX12 10

#define C_MUX21 0
#define C_MUX22 1

#define C_MUX_S0 5
#define C_MUX_S1 6
#define C_MUX_S2 7
#define C_MUX_S3 8


void mux4067_init() {
    // last_mux = 0;

    gpio_init(C_MUX11);
    gpio_init(C_MUX12);
    gpio_init(C_MUX21);
    gpio_init(C_MUX22);

    gpio_init(C_MUX_S0);
    gpio_init(C_MUX_S1);
    gpio_init(C_MUX_S2);
    gpio_init(C_MUX_S3);

    gpio_set_dir(C_MUX11, false);  // set to input
    gpio_set_dir(C_MUX12, false);  // set to input
    gpio_set_dir(C_MUX21, false);  // set to input
    gpio_set_dir(C_MUX22, false);  // set to input

    // pull ups
    gpio_pull_up(C_MUX11);
    gpio_pull_up(C_MUX12);
    gpio_pull_up(C_MUX21);
    gpio_pull_up(C_MUX22);

    gpio_set_dir(C_MUX_S0, true);  // set to output
    gpio_set_dir(C_MUX_S1, true);  // set to output
    gpio_set_dir(C_MUX_S2, true);  // set to output
    gpio_set_dir(C_MUX_S3, true);  // set to output
}

void mux4067_update(uint8_t mux_p1, uint8_t mux_p2) {
    uint32_t m1, m2;
    for (int i = 15; i >= 0; i--) {
        // set the selector pins to the current value
        gpio_put(C_MUX_S3, (i >> 3) & 1);
        gpio_put(C_MUX_S2, (i >> 2) & 1);
        gpio_put(C_MUX_S1, (i >> 1) & 1);
        gpio_put(C_MUX_S0, i & 1);

        busy_wait_us(WAIT_INPUT_MUX4067);  // wait for selector to change

        // // select mux based on if the input is related to P1 or P2 mux or neither
        // uint8_t mux1 = MUX_GLOBAL;
        // uint8_t mux2 = MUX_GLOBAL;
        // for (int j = 0; j < MUX_P1_INPUTS_SIZE; j++) {
        //     if (i == mux_p1_inputs[j]) {
        //         mux1 = mux_p1;
        //     } else if (i + 16 == mux_p1_inputs[j]) {
        //         mux2 = mux_p1;
        //     }
        // }
        // for (int j = 0; j < MUX_P2_INPUTS_SIZE; j++) {
        //     if (i == mux_p2_inputs[j]) {
        //         mux1 = mux_p2;
        //     } else if (i + 16 == mux_p2_inputs[j]) {
        //         mux2 = mux_p2;
        //     }
        // }



        // // read this value from BOTH muxes and store them in the ith bit and the (i+16)th bit respectively
        // #ifdef PULLUP_IN
        //     SETORCLRBIT(mux4067_vals[mux1], i, !gpio_get(MUX1_IN_PIN));
        //     SETORCLRBIT(mux4067_vals[mux2], i+16, !gpio_get(MUX2_IN_PIN));
        // #else
        //     SETORCLRBIT(mux4067_vals[mux1], i, gpio_get(MUX1_IN_PIN));
        //     SETORCLRBIT(mux4067_vals[mux2], i+16, gpio_get(MUX2_IN_PIN));
        // #endif

        SETORCLRBIT(m1, i, !gpio_get(C_MUX11));
        SETORCLRBIT(m1, i + 16, !gpio_get(C_MUX12));
        SETORCLRBIT(m2, i, !gpio_get(C_MUX21));
        SETORCLRBIT(m2, i + 16, !gpio_get(C_MUX22));
    }

    // DR
    SETORCLRBIT(mux4067_vals[0], MUX4067_P1_DOWNRIGHT, GETBIT(m1, 0));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P1_DOWNRIGHT, GETBIT(m1, 1));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P1_DOWNRIGHT, GETBIT(m1, 2));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P1_DOWNRIGHT, GETBIT(m1, 3));
    // UR
    SETORCLRBIT(mux4067_vals[0], MUX4067_P1_UPRIGHT, GETBIT(m1, 4));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P1_UPRIGHT, GETBIT(m1, 5));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P1_UPRIGHT, GETBIT(m1, 6));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P1_UPRIGHT, GETBIT(m1, 7));
    // CN
    SETORCLRBIT(mux4067_vals[0], MUX4067_P1_CENTER, GETBIT(m1, 8));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P1_CENTER, GETBIT(m1, 9));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P1_CENTER, GETBIT(m1, 10));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P1_CENTER, GETBIT(m1, 11));
    // UL -> DL
    SETORCLRBIT(mux4067_vals[0], MUX4067_P1_UPLEFT, GETBIT(m1, 16));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P1_UPLEFT, GETBIT(m1, 17));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P1_UPLEFT, GETBIT(m1, 18));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P1_UPLEFT, GETBIT(m1, 19));
    // DL -> UL
    SETORCLRBIT(mux4067_vals[0], MUX4067_P1_DOWNLEFT, GETBIT(m1, 12));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P1_DOWNLEFT, GETBIT(m1, 13));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P1_DOWNLEFT, GETBIT(m1, 14));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P1_DOWNLEFT, GETBIT(m1, 15));

    // DR
    SETORCLRBIT(mux4067_vals[0], MUX4067_P2_DOWNRIGHT, GETBIT(m2, 0));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P2_DOWNRIGHT, GETBIT(m2, 1));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P2_DOWNRIGHT, GETBIT(m2, 2));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P2_DOWNRIGHT, GETBIT(m2, 3));
    // UR
    SETORCLRBIT(mux4067_vals[0], MUX4067_P2_UPRIGHT, GETBIT(m2, 4));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P2_UPRIGHT, GETBIT(m2, 5));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P2_UPRIGHT, GETBIT(m2, 6));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P2_UPRIGHT, GETBIT(m2, 7));
    // CN
    SETORCLRBIT(mux4067_vals[0], MUX4067_P2_CENTER, GETBIT(m2, 8));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P2_CENTER, GETBIT(m2, 9));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P2_CENTER, GETBIT(m2, 10));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P2_CENTER, GETBIT(m2, 11));
    // UL -> DL
    SETORCLRBIT(mux4067_vals[0], MUX4067_P2_UPLEFT, GETBIT(m2, 16));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P2_UPLEFT, GETBIT(m2, 17));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P2_UPLEFT, GETBIT(m2, 18));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P2_UPLEFT, GETBIT(m2, 19));
    // DL -> UL
    SETORCLRBIT(mux4067_vals[0], MUX4067_P2_DOWNLEFT, GETBIT(m2, 12));
    SETORCLRBIT(mux4067_vals[1], MUX4067_P2_DOWNLEFT, GETBIT(m2, 13));
    SETORCLRBIT(mux4067_vals[2], MUX4067_P2_DOWNLEFT, GETBIT(m2, 14));
    SETORCLRBIT(mux4067_vals[3], MUX4067_P2_DOWNLEFT, GETBIT(m2, 15));
    
    // globals
    // handled via HID report in host_hid.c
    // SETORCLRBIT(mux4067_vals[4], MUX4067_TEST, GETBIT(m2, 20));
    // SETORCLRBIT(mux4067_vals[4], MUX4067_SERVICE, GETBIT(m2, 21));
    // SETORCLRBIT(mux4067_vals[4], MUX4067_CLEAR, GETBIT(m2, 22));
    // SETORCLRBIT(mux4067_vals[4], MUX4067_P1_COIN, GETBIT(m2, 23));
    // SETORCLRBIT(mux4067_vals[4], MUX4067_P2_COIN, GETBIT(m2, 24));
/*

#define MUX4067_P1_UPLEFT 23
#define MUX4067_P1_UPRIGHT 22
#define MUX4067_P1_CENTER 21
#define MUX4067_P1_DOWNLEFT 20
#define MUX4067_P1_DOWNRIGHT 19

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

*/
}

uint32_t mux4067_merged(uint32_t* vals) {
    return vals[0] | vals[1] | vals[2] | vals[3] | vals[4];
}

void mux4067_debounce() {
    #if defined(DEBOUNCING)
    uint32_t current_ts = board_millis();
    
    for (int mux = 0; mux < MUX_COUNT; mux++) {
        for (int i = 0; i < 32; i++) {
            uint32_t state = GETBIT(mux4067_vals[mux], i);
            
            if (state != (GETBIT(mux4067_vals_last[mux], i))) {
                // button state has changed
                if (state) {
                    press_ts[mux][i] = current_ts;
                } else {
                    release_ts[mux][i] = current_ts;
                }
            } else if (state && (current_ts - press_ts[mux][i]) >= DEBOUNCE_PRESS_TIME) {
                // check if the button has been held for debounce time
                SETBIT(mux4067_vals_db[mux], i); // set debounced button state
            } else if (!state && (current_ts - release_ts[mux][i]) >= DEBOUNCE_RELEASE_TIME) {
                CLRBIT(mux4067_vals_db[mux], i);
            }
        }
        mux4067_vals_last[mux] = mux4067_vals[mux]; // store current button state for next iteration
    }
    #else
    memcpy(mux4067_vals_db, mux4067_vals, sizeof(mux4067_vals));
    #endif
}