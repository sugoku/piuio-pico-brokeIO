/*******************************************************************************************/
/*  SPDX-License-Identifier: MIT                                                           */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 48productions, therathatter, dj505, sugoku  */
/*  https://github.com/sugoku/piuio-pico-brokeIO                                           */
/*******************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "bsp/board.h"
#include "device/usbd.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"

#include "piuio_structs.h"
#include "piuio_config.h"
#include "input_mode.h"

// #include "uart_structs.h"

#include "input_mux4067.h"
#include "lights_latch32.h"

#include "reports/hid_report.h"
#include "reports/keyboard_report.h"
#include "reports/lxio_report.h"
#include "reports/switch_report.h"
#include "reports/xinput_report.h"
#include "reports/gamecube_report.h"

// please fix all of this CDC code
enum
{
  ITF_NUM_CDC_0 = 0,
  ITF_NUM_CDC_0_DATA,
  ITF_NUM_TOTAL_SERIAL
};

#include "xinput_driver.h"
#include "device/usbd_pvt.h"

#ifdef ENABLE_WS2812_SUPPORT
#include "piuio_ws2812.h"
#endif

// const uint8_t pos[] = { 3, 0, 2, 1, 4 }; // don't touch this
// i touched it

int input_mode = -1;
int input_mode_tmp = -1;

bool config_mode = false;
bool config_switched = false;

// set pad lights based on whether an arrow is pressed or not, bypassing the host
// automatically enabled for all modes besides PIUIO and LXIO
bool direct_lights = false;

// have brokeIO automatically switch mux between reads instead of waiting for
// host signal (required for everything except PIUIO)
bool auto_mux = false;

// merge all sensors for a panel, so that the host receives all sensor readings
// ORed together (stepping on at least one sensor always triggers the arrow)
bool merge_mux = false;

// this mode is used to confirm the functionality of a brokeIO
// it maps each input in the 4067 mux to its corresponding output on the latch
const bool factory_test_mode = false;

// is JAMMA_Q toggled on or off
bool q_toggle = false;

bool last_q = false;

bool jamma_w = false;
bool jamma_x = false;
bool jamma_y = false;
bool jamma_z = false;

// used for auto mux mode
uint8_t current_mux = 0;

uint32_t serial_lights_buf;

uint32_t last_service_ts;
uint32_t last_p1_cn_ts;
uint32_t last_p2_cn_ts;
uint8_t hid_rx_buf[32];

extern uint8_t xinput_out_buffer[XINPUT_OUT_SIZE];

// PIUIO input and output data
// note that inputs are ACTIVE LOW
struct inputArray input = {
    .data = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

struct inputArray last_input = {
    .data = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

// use this array for PIUIO when all inputs should be off
const uint8_t all_inputs_off[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct inputArray input_mux[MUX_COUNT];

// note that lights array are ACTIVE HIGH
struct lightsArray lights = {
    .data = {0x00}
};
// this is a lights array that gets written to when data is still being transferred.
// once data is finished transferring it should copy its contents to the real lights array
struct lightsArray temp_lights = {
    .data = {0x00}
};

extern uint32_t mux4067_vals_db[MUX_COUNT];

void update_input_mux() {
    uint32_t buf_mux_global;
    uint32_t buf_mux_p1;
    uint32_t buf_mux_p2;
    
    if (merge_mux || config_mode) {
        uint32_t merged = mux4067_merged(mux4067_vals_db);
        buf_mux_global = merged;
        buf_mux_p1 = merged;
        buf_mux_p2 = merged;
    } else {
        buf_mux_global = mux4067_vals_db[MUX_GLOBAL];
        buf_mux_p1 = mux4067_vals_db[lights.p1_mux];
        buf_mux_p2 = mux4067_vals_db[lights.p2_mux];
    }
    
    // logic is negative
    input.p1_ul = !GETBIT(buf_mux_p1, MUX4067_P1_UPLEFT);
    input.p1_ur = !GETBIT(buf_mux_p1, MUX4067_P1_UPRIGHT);
    input.p1_cn = !GETBIT(buf_mux_p1, MUX4067_P1_CENTER);
    input.p1_dl = !GETBIT(buf_mux_p1, MUX4067_P1_DOWNLEFT);
    input.p1_dr = !GETBIT(buf_mux_p1, MUX4067_P1_DOWNRIGHT);

    input.p2_ul = !GETBIT(buf_mux_p2, MUX4067_P2_UPLEFT);
    input.p2_ur = !GETBIT(buf_mux_p2, MUX4067_P2_UPRIGHT);
    input.p2_cn = !GETBIT(buf_mux_p2, MUX4067_P2_CENTER);
    input.p2_dl = !GETBIT(buf_mux_p2, MUX4067_P2_DOWNLEFT);
    input.p2_dr = !GETBIT(buf_mux_p2, MUX4067_P2_DOWNRIGHT);

    input.p1_coin = !GETBIT(buf_mux_global, MUX4067_P1_COIN);
    input.p2_coin = !GETBIT(buf_mux_global, MUX4067_P2_COIN);

    input.test = !GETBIT(buf_mux_global, MUX4067_TEST);
    input.service = !GETBIT(buf_mux_global, MUX4067_SERVICE);
    input.clear = !GETBIT(buf_mux_global, MUX4067_CLEAR);

    for (int i = 0; i < MUX_COUNT; i++) {
        // logic is negative
        input_mux[i].p1_ul = !GETBIT(mux4067_vals_db[i], MUX4067_P1_UPLEFT);
        input_mux[i].p1_ur = !GETBIT(mux4067_vals_db[i], MUX4067_P1_UPRIGHT);
        input_mux[i].p1_cn = !GETBIT(mux4067_vals_db[i], MUX4067_P1_CENTER);
        input_mux[i].p1_dl = !GETBIT(mux4067_vals_db[i], MUX4067_P1_DOWNLEFT);
        input_mux[i].p1_dr = !GETBIT(mux4067_vals_db[i], MUX4067_P1_DOWNRIGHT);

        input_mux[i].p2_ul = !GETBIT(mux4067_vals_db[i], MUX4067_P2_UPLEFT);
        input_mux[i].p2_ur = !GETBIT(mux4067_vals_db[i], MUX4067_P2_UPRIGHT);
        input_mux[i].p2_cn = !GETBIT(mux4067_vals_db[i], MUX4067_P2_CENTER);
        input_mux[i].p2_dl = !GETBIT(mux4067_vals_db[i], MUX4067_P2_DOWNLEFT);
        input_mux[i].p2_dr = !GETBIT(mux4067_vals_db[i], MUX4067_P2_DOWNRIGHT);

        input_mux[i].p1_coin = !GETBIT(mux4067_vals_db[i], MUX4067_P1_COIN);
        input_mux[i].p2_coin = !GETBIT(mux4067_vals_db[i], MUX4067_P2_COIN);

        input_mux[i].test = !GETBIT(mux4067_vals_db[i], MUX4067_TEST);
        input_mux[i].service = !GETBIT(mux4067_vals_db[i], MUX4067_SERVICE);
        input_mux[i].clear = !GETBIT(mux4067_vals_db[i], MUX4067_CLEAR);
    }
}

void input_task() {
    uint32_t current_ts = board_millis();

    mux4067_update(lights.p1_mux, lights.p2_mux);
    mux4067_debounce();

    update_input_mux();

    uint32_t merged = mux4067_merged(mux4067_vals_db);

    // read extra unused I/O for supported modes
    bool q = GETBIT(merged, MUX4067_JAMMA_17);
    if (q && !last_q) {
        q_toggle = !q_toggle;
    }

    jamma_w = GETBIT(merged, MUX4067_JAMMA_W);
    jamma_x = GETBIT(merged, MUX4067_JAMMA_X);
    jamma_y = GETBIT(merged, MUX4067_JAMMA_Y);
    jamma_z = GETBIT(merged, MUX4067_JAMMA_Z);

    if (config_mode) {
        if ((!input.p1_dl && last_input.p1_dl) || (!input.p2_dl && last_input.p2_dl)) {
            if (input_mode_tmp > 0)
                input_mode_tmp--;
        } else if ((!input.p1_dr && last_input.p1_dr) || (!input.p2_dr && last_input.p2_dr)) {
            if (input_mode_tmp < INPUT_MODE_COUNT - 1)
                input_mode_tmp++;
        } else if ((!input.test && last_input.test) || (!input.test && last_input.test)) {
            input_mode_tmp = (input_mode_tmp + 1) % INPUT_MODE_COUNT;
        }
    }

    // keep track of last time service button was held down
    // also don't do this if test is also pressed down
    if (last_input.service && !input.service && input.test) {
        last_service_ts = current_ts;
        config_switched = true;
    }
    // if button held down long enough, enter or exit config mode
    if (config_switched && !input.service && current_ts - last_service_ts > SETTINGS_THRESHOLD && input.test) {
        if (!config_mode) {
            input_mode_tmp = input_mode;
            config_mode = true;
        } else {
            config_mode = false;
            // save changes for mode to flash memory and reset device!
            write_input_mode(input_mode_tmp);
            // enable watchdog and enter infinite loop to reset
            watchdog_enable(1, 1);
            while(1);
        }
        config_switched = false;
        // use config_switched to make sure that when we enter config_mode
        // we don't instantly exit it (requiring another button press to switch config_mode)
    }

    // enter usb bootloader mode (be careful using in production!)
    if ((config_mode || ALWAYS_BOOTLOADER) && !input.p2_ul && !input.p2_ur && !input.p2_dr) {
        reset_usb_boot(0, 0);
    }

    last_input = input;
    last_q = q;
}

void config_mode_led_update(uint32_t* buf) {
    uint32_t time = board_millis();
    uint8_t blink_count = (time / SERVICE_BLINK_INTERVAL) % (input_mode + 1);
    // blink for BLINK_LENGTH every BLINK_INTERVAL ms for input_mode times on the cab, but blink constantly for pad
    // then leave an empty spot at the end to separate the next blinking cycle
    bool pad_state = (time % SERVICE_BLINK_INTERVAL <= SERVICE_BLINK_LENGTH);
    bool state = (blink_count <= input_mode_tmp) && pad_state;

    SETORCLRBIT(*buf, LATCH_JAMMA_LED, state);

    SETORCLRBIT(*buf, LATCH_P1L_UPLEFT, pad_state && (input_mode_tmp & 0b100));
    SETORCLRBIT(*buf, LATCH_P1L_CENTER, pad_state && (input_mode_tmp & 0b10));
    SETORCLRBIT(*buf, LATCH_P1L_UPRIGHT, pad_state && (input_mode_tmp & 0b1));

    SETORCLRBIT(*buf, LATCH_P2L_UPLEFT, pad_state && (input_mode_tmp & 0b100));
    SETORCLRBIT(*buf, LATCH_P2L_CENTER, pad_state && (input_mode_tmp & 0b10));
    SETORCLRBIT(*buf, LATCH_P2L_UPRIGHT, pad_state && (input_mode_tmp & 0b1));

    SETORCLRBIT(*buf, LATCH_CABL_MARQ1, state && (input_mode_tmp & 0b100));
    SETORCLRBIT(*buf, LATCH_CABL_MARQ2, state && (input_mode_tmp & 0b10));
    SETORCLRBIT(*buf, LATCH_CABL_MARQ3, state && (input_mode_tmp & 0b1));
}

void lights_task() {
    #ifdef ENABLE_WS2812_SUPPORT
    ws2812_lock_mtx();
    #endif

    uint32_t buf = 0;

    if (config_mode) {
        // force direct_lights behavior
        uint32_t in_buf = mux4067_merged(mux4067_vals_db);
        
        SETORCLRBIT(buf, LATCH_P1L_DOWNLEFT, GETBIT(in_buf, MUX4067_P1_DOWNLEFT));
        SETORCLRBIT(buf, LATCH_P1L_DOWNRIGHT, GETBIT(in_buf, MUX4067_P1_DOWNRIGHT));

        SETORCLRBIT(buf, LATCH_P2L_DOWNLEFT, GETBIT(in_buf, MUX4067_P2_DOWNLEFT));
        SETORCLRBIT(buf, LATCH_P2L_DOWNRIGHT, GETBIT(in_buf, MUX4067_P2_DOWNRIGHT));

        SETORCLRBIT(buf, LATCH_P1_S0, lights.p1_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P1_S1, lights.p1_mux & 0b10);
        SETORCLRBIT(buf, LATCH_P2_S0, lights.p2_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P2_S1, lights.p2_mux & 0b10);

        SETBIT(buf, LATCH_CABL_MARQ4);
        SETBIT(buf, LATCH_CABL_NEON);

        SETBIT(buf, LATCH_ALWAYS_ON);

        config_mode_led_update(&buf);
        
    } else if (factory_test_mode) {
        buf = mux4067_merged(mux4067_vals_db);
    } else if (input_mode == INPUT_MODE_SERIAL) {
        buf = serial_lights_buf;
    } else if (direct_lights) {  // technically it could be direct_lights && !merge_mux
        uint32_t in_buf = mux4067_merged(mux4067_vals_db);

        SETORCLRBIT(buf, LATCH_P1L_UPLEFT, GETBIT(in_buf, MUX4067_P1_UPLEFT));
        SETORCLRBIT(buf, LATCH_P1L_UPRIGHT, GETBIT(in_buf, MUX4067_P1_UPRIGHT));
        SETORCLRBIT(buf, LATCH_P1L_CENTER, GETBIT(in_buf, MUX4067_P1_CENTER));
        SETORCLRBIT(buf, LATCH_P1L_DOWNLEFT, GETBIT(in_buf, MUX4067_P1_DOWNLEFT));
        SETORCLRBIT(buf, LATCH_P1L_DOWNRIGHT, GETBIT(in_buf, MUX4067_P1_DOWNRIGHT));

        SETORCLRBIT(buf, LATCH_P2L_UPLEFT, GETBIT(in_buf, MUX4067_P2_UPLEFT));
        SETORCLRBIT(buf, LATCH_P2L_UPRIGHT, GETBIT(in_buf, MUX4067_P2_UPRIGHT));
        SETORCLRBIT(buf, LATCH_P2L_CENTER, GETBIT(in_buf, MUX4067_P2_CENTER));
        SETORCLRBIT(buf, LATCH_P2L_DOWNLEFT, GETBIT(in_buf, MUX4067_P2_DOWNLEFT));
        SETORCLRBIT(buf, LATCH_P2L_DOWNRIGHT, GETBIT(in_buf, MUX4067_P2_DOWNRIGHT));

        SETORCLRBIT(buf, LATCH_P1_S0, lights.p1_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P1_S1, lights.p1_mux & 0b10);
        SETORCLRBIT(buf, LATCH_P2_S0, lights.p2_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P2_S1, lights.p2_mux & 0b10);
        
        SETORCLRBIT(buf, LATCH_CABL_MARQ1, lights.l1_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ2, lights.l2_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ3, lights.r1_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ4, lights.r2_halo);
        SETORCLRBIT(buf, LATCH_CABL_NEON, lights.bass_light);

        SETBIT(buf, LATCH_ALWAYS_ON);
        SETORCLRBIT(buf, LATCH_COIN_COUNTER, lights.coin_pulse);
        SETBIT(buf, LATCH_JAMMA_LED);
    } else {
        SETORCLRBIT(buf, LATCH_P1L_UPLEFT, lights.p1_ul_light);
        SETORCLRBIT(buf, LATCH_P1L_UPRIGHT, lights.p1_ur_light);
        SETORCLRBIT(buf, LATCH_P1L_CENTER, lights.p1_cn_light);
        SETORCLRBIT(buf, LATCH_P1L_DOWNLEFT, lights.p1_dl_light);
        SETORCLRBIT(buf, LATCH_P1L_DOWNRIGHT, lights.p1_dr_light);

        SETORCLRBIT(buf, LATCH_P2L_UPLEFT, lights.p2_ul_light);
        SETORCLRBIT(buf, LATCH_P2L_UPRIGHT, lights.p2_ur_light);
        SETORCLRBIT(buf, LATCH_P2L_CENTER, lights.p2_cn_light);
        SETORCLRBIT(buf, LATCH_P2L_DOWNLEFT, lights.p2_dl_light);
        SETORCLRBIT(buf, LATCH_P2L_DOWNRIGHT, lights.p2_dr_light);

        SETORCLRBIT(buf, LATCH_P1_S0, lights.p1_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P1_S1, lights.p1_mux & 0b10);
        SETORCLRBIT(buf, LATCH_P2_S0, lights.p2_mux & 0b1);
        SETORCLRBIT(buf, LATCH_P2_S1, lights.p2_mux & 0b10);

        SETORCLRBIT(buf, LATCH_CABL_MARQ1, lights.l1_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ2, lights.l2_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ3, lights.r1_halo);
        SETORCLRBIT(buf, LATCH_CABL_MARQ4, lights.r2_halo);
        SETORCLRBIT(buf, LATCH_CABL_NEON, lights.bass_light);

        SETBIT(buf, LATCH_ALWAYS_ON);
        SETORCLRBIT(buf, LATCH_COIN_COUNTER, lights.coin_pulse);
        SETBIT(buf, LATCH_JAMMA_LED);
    }

    lights_send(&buf);
    

    #ifdef ENABLE_WS2812_SUPPORT
    ws2812_unlock_mtx();
    #endif

    if (auto_mux || config_mode) {
        current_mux = (current_mux + 1) % 4;
        lights.p1_mux = current_mux;
        lights.p2_mux = current_mux;
    }
}

void receive_report(uint8_t *buffer) {
    if (input_mode == INPUT_MODE_XINPUT) {
        receive_xinput_report();
        memcpy(buffer, xinput_out_buffer, XINPUT_OUT_SIZE);
    }
}

void send_report(void *report, uint16_t report_size) {
    static uint8_t previous_report[CFG_TUD_ENDPOINT0_SIZE] = { };

    if (report_size == 0 || report == NULL)
        return;

    if (tud_suspended())
        tud_remote_wakeup();

    if (memcmp(previous_report, report, report_size) != 0 || ((input_mode == INPUT_MODE_LXIO) && LXIO_ALWAYS_SEND_REPORT)) {
        bool sent = false;
        switch (input_mode) {
            case INPUT_MODE_XINPUT:
                sent = send_xinput_report(report, report_size);
                break;

            default:
                if (tud_hid_ready())
                    sent = tud_hid_report(0, report, report_size);
                break;
        }

        if (sent)
            memcpy(previous_report, report, report_size);
    }
}

// returns size, sets pointer to report pointer
// we need double pointers to ensure that we change the address of void*
// and that this change persists outside this function
uint16_t get_report(void** report) {
    switch (input_mode) {
        case INPUT_MODE_GAMEPAD:
            return hid_get_report((HIDReport**)report, &input);

        case INPUT_MODE_LXIO:
            return lxio_get_report((uint8_t**)report, &input, input_mux);

        case INPUT_MODE_KEYBOARD:
            return keyboard_get_report((KeyboardReport**)report, &input);

        case INPUT_MODE_XINPUT:
            return xinput_get_report((XInputReport**)report, &input);

        case INPUT_MODE_SWITCH:
            return switch_get_report((SwitchReport**)report, &input, q_toggle, jamma_w, jamma_x, jamma_y, jamma_z);

        case INPUT_MODE_GAMECUBE:
            return gamecube_get_report((GameCubeReport**)report, &input, &last_input, q_toggle, jamma_w, jamma_x, jamma_y, jamma_z);

        default:
            return 0;
    }
}

void hid_task() {
    if (config_mode || input_mode == INPUT_MODE_PIUIO)
        return;

    // move to cdc_task
    if (input_mode == INPUT_MODE_SERIAL) {
        if (tud_cdc_n_available(ITF_NUM_CDC_0)) {
            uint8_t buf[64] = {0};
            uint32_t count = tud_cdc_n_read(ITF_NUM_CDC_0, buf, sizeof(buf));
            
            if (count > 0) {
                int bit = atoi(buf);
                SETORCLRBIT(serial_lights_buf, bit, !GETBIT(serial_lights_buf, bit));
            }
        }

        uint32_t merged = mux4067_merged(mux4067_vals_db);
        for (int i = 31; i >= 0; i--) {
            tud_cdc_n_write_char(ITF_NUM_CDC_0, '0'+((merged >> i) & 1));
            if (i % 8 == 0) {
                tud_cdc_n_write_char(ITF_NUM_CDC_0, ',');
            }
        }
        tud_cdc_n_write_char(ITF_NUM_CDC_0, '\n');
        tud_cdc_n_write_flush(ITF_NUM_CDC_0);
        return;
    }

    // USB FEATURES : Send/Get USB Features (including Player LEDs on X-Input)
    void* report = NULL;
    uint16_t size = get_report(&report);
    send_report(report, size);
    receive_report(hid_rx_buf);
}

void init() {
    get_input_mode();

    switch (input_mode) {
        case INPUT_MODE_PIUIO:
            direct_lights = false;
            auto_mux = false;
            merge_mux = MERGE_MUX_PIUIO;
            break;

        case INPUT_MODE_GAMEPAD:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;

        case INPUT_MODE_LXIO:
            direct_lights = false;
            auto_mux = true;
            merge_mux = false;
            break;

        case INPUT_MODE_KEYBOARD:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;

        case INPUT_MODE_XINPUT:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;

        case INPUT_MODE_SWITCH:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;

        case INPUT_MODE_GAMECUBE:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;

        case INPUT_MODE_SERIAL:
            direct_lights = true;
            auto_mux = true;
            merge_mux = true;
            break;
    }

    #ifdef BENCHMARK
    gpio_init(BENCHMARK_PIN_1);
    gpio_init(BENCHMARK_PIN_2);
    gpio_init(BENCHMARK_PIN_3);
    gpio_init(BENCHMARK_PIN_4);

    gpio_set_dir(BENCHMARK_PIN_1, true);  // set benchmark pin as output
    gpio_set_dir(BENCHMARK_PIN_2, true);  // set benchmark pin as output
    gpio_set_dir(BENCHMARK_PIN_3, true);  // set benchmark pin as output
    gpio_set_dir(BENCHMARK_PIN_4, true);  // set benchmark pin as output
    #endif
}

int main() {
    board_init();

    // multicore_launch_core1(core1_entry);

    // Init WS2812B
    #ifdef ENABLE_WS2812_SUPPORT
    ws2812_init(&lights);
    #endif

    // this is a delay added in case any components are not ready to start yet
    sleep_ms(100);

    mux4067_init();
    lights_init();

    init();

    // this is a delay added in case USB is not ready to start yet
    sleep_ms(100);

    tusb_init();

    #ifdef BENCHMARK
    uint8_t loop_toggle = 0x00;
    #endif

    // Main loop
    while (true) {

        #ifdef BENCHMARK
        // every loop, toggle the benchmark pin
        // we can then monitor the time each loop takes using an oscilloscope
        loop_toggle ^= 0x01;
        gpio_put(BENCHMARK_PIN_1, loop_toggle);
        #endif

        tud_task(); // tinyusb device task

        lights_task();  // update mux before reading in input
        input_task();

        hid_task();

        // uart_task();
    }

    return 0;
}


// ------------ tinyusb callbacks ------------

const usbd_class_driver_t *usbd_app_driver_get_cb(uint8_t *driver_count)
{
    // only switch the driver when we are using xinput; otherwise, use defaults and do nothing

    if (input_mode == INPUT_MODE_XINPUT) {
        *driver_count = 1;
        return &xinput_driver;
    }
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request) {
    // nothing to with DATA & ACK stage
    if (input_mode == INPUT_MODE_PIUIO) {
        
        #ifdef BENCHMARK
        static uint8_t loop_toggle_out = 0x00;
        static uint8_t loop_toggle_in = 0x00;
        #endif

        // Request 0xAE = IO Time
        if (request->bRequest == 0xAE) {
            switch (request->bmRequestType_bit.direction) {
                case TUSB_DIR_OUT:  // lights

                    #ifdef BENCHMARK
                    if (stage == CONTROL_STAGE_SETUP) {
                        loop_toggle_out ^= 0x01;
                        gpio_put(BENCHMARK_PIN_2, loop_toggle_out);
                    }
                    #endif

                    if (config_mode)
                        return false;
                    if (stage == CONTROL_STAGE_SETUP)
                        return tud_control_xfer(rhport, request, (void *)&temp_lights.data, sizeof(temp_lights.data));
                    if (stage == CONTROL_STAGE_DATA)
                        memcpy(&lights.data, &temp_lights.data, sizeof(temp_lights.data));
                    if (stage == CONTROL_STAGE_ACK)
                        return true;

                case TUSB_DIR_IN:  // input

                    #ifdef BENCHMARK
                    if (stage == CONTROL_STAGE_SETUP) {
                        loop_toggle_in ^= 0x01;
                        gpio_put(BENCHMARK_PIN_3, loop_toggle_in);
                    }
                    #endif

                    // if in config mode, make sure that we turn all inputs off or inputs will get stuck!
                    if (config_mode)
                        return tud_control_xfer(rhport, request, (void *)all_inputs_off, sizeof(input.data));
                    if (stage == CONTROL_STAGE_SETUP)
                        return tud_control_xfer(rhport, request, (void *)&input.data, sizeof(input.data));
                    if (stage == CONTROL_STAGE_DATA || stage == CONTROL_STAGE_ACK)
                        return true;
                default:
                    return false;
            }
        }

        return true;
    }

    return false;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO: Handle the correct report type, if required
    (void)itf;

    if (config_mode || input_mode == INPUT_MODE_PIUIO) return 0;

    void* report = NULL;
    uint16_t size = get_report(&report);

    if (size == 0 || report == NULL)
        return 0;
        
    memcpy(buffer, report, size);

    return size;
}

// set_report needed here for HID lights and LXIO
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void) instance;

    if (!config_mode) {
        if (input_mode == INPUT_MODE_LXIO) {
            // if (report_type == HID_REPORT_TYPE_OUTPUT) {
            // do not consider the report type at all! tinyusb ignores it and sets it to HID_REPORT_TYPE_INVALID (0)
            // also note that no report ID is specified in the LXIO's device descriptor
            lxio_set_report(buffer, bufsize, &lights);
        } else if (input_mode == INPUT_MODE_GAMECUBE) {
            // rumble
        } else if (input_mode == INPUT_MODE_GAMEPAD) {
            hid_set_report(buffer, bufsize, &lights);
        }
    }
}