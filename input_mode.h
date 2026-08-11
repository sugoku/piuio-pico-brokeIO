/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#ifndef _INPUT_MODE_H
#define _INPUT_MODE_H

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "piuio_config.h"

extern int input_mode;

int get_input_mode();
uint8_t read_input_mode();

/**
 * Persist the input mode to flash.
 *
 * Stops core1 first, because erasing flash stalls XIP and would fault any core
 * still executing from it. The caller is expected to reset the chip afterwards.
 */
void write_input_mode(uint8_t value);

#endif