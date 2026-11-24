/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#include "input_mode.h"
#include <string.h>

mutable_config_t config;
bool config_loaded = false;

mutable_config_t* load_config() {
    if (!config_loaded) {
        #if defined(ALWAYS_DEFAULT_INPUT_MODE)
            config.input_mode = DEFAULT_INPUT_MODE;
            config.swap_pad_lights = 0;
        #else
            // read from flash memory directly into struct
            memcpy(&config, (const void *)(XIP_BASE + INPUT_MODE_OFFSET), sizeof(mutable_config_t));

            // bounds checking
            if (config.input_mode >= INPUT_MODE_COUNT)
                config.input_mode = DEFAULT_INPUT_MODE;
            if (config.swap_pad_lights > 1)
                config.swap_pad_lights = 0;
        #endif
        config_loaded = true;
    }

    return &config;
}

// thanks to Kevin Boone for the helpful information!
// https://kevinboone.me/picoflash.html

void write_config(const mutable_config_t* new_config) {
    #ifndef ALWAYS_DEFAULT_INPUT_MODE
    get_config();
    if (memcmp(&config, new_config, sizeof(mutable_config_t)) == 0) {
        return;
    }

    // at minimum we have to write FLASH_PAGE_SIZE bytes (256)
    // this sets the values in the array and the rest should be 0
    uint8_t buf[FLASH_PAGE_SIZE] = {0};
    memcpy(buf, new_config, sizeof(mutable_config_t));

    // disabling interrupts is required before writing to flash!
    uint32_t ints = save_and_disable_interrupts();

    // we erase the sector before we write to it
    flash_range_erase(INPUT_MODE_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(INPUT_MODE_OFFSET, buf, FLASH_PAGE_SIZE);

    // we can restore interrupts afterwards
    restore_interrupts(ints);

    // update cached config
    memcpy(&config, new_config, sizeof(mutable_config_t));
    #endif
}
