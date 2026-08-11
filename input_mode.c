/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#include "input_mode.h"

#include "pico/multicore.h"

int get_input_mode() {
    #if defined(ALWAYS_DEFAULT_INPUT_MODE)
        input_mode = DEFAULT_INPUT_MODE;
    #else
        if (input_mode < 0)
            input_mode = read_input_mode();
        
        if (input_mode >= INPUT_MODE_COUNT)
            input_mode = DEFAULT_INPUT_MODE;
    #endif

    return input_mode;
}

// thanks to Kevin Boone for the helpful information!
// https://kevinboone.me/picoflash.html

uint8_t read_input_mode() {
    // read byte at offset in memory where XIP_BASE is flash address start
    return *(uint8_t *)(XIP_BASE + INPUT_MODE_OFFSET);
}

void write_input_mode(uint8_t value) {
    #ifndef ALWAYS_DEFAULT_INPUT_MODE
    // at minimum we have to write FLASH_PAGE_SIZE bytes (256)
    // this sets the first value in the array and the rest should be 0
    uint8_t buf[FLASH_PAGE_SIZE] = {value};

    // Erasing flash stalls XIP, so any core fetching instructions from flash
    // during the erase faults or executes garbage. save_and_disable_interrupts
    // only covers the calling core, and core1 is sitting in tuh_task() out of
    // flash, so it has to be stopped outright. Callers reset the chip straight
    // after this, so there is nothing to restart.
    multicore_reset_core1();

    // disabling interrupts is required before writing to flash!
    uint32_t ints = save_and_disable_interrupts();

    // we erase the sector before we write to it
    flash_range_erase(INPUT_MODE_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(INPUT_MODE_OFFSET, buf, FLASH_PAGE_SIZE);

    // we can restore interrupts afterwards
    restore_interrupts(ints);
    #endif
};