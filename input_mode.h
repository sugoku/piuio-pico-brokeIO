/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#ifndef _INPUT_MODE_H
#define _INPUT_MODE_H

#include <stdbool.h>
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "piuio_config.h"

typedef struct {
    // The current input mode (eg: PIUIO, XInput, Switch, Gamecube, etc).
    uint8_t input_mode;

    // For ITG dedicabs; swap 1P and 2P lights in non-PIUIO modes.
    uint8_t swap_pad_lights;
} mutable_config_t;

extern mutable_config_t config;
extern bool config_loaded;

mutable_config_t* load_config();

static inline const mutable_config_t* get_config() {
    return config_loaded ? &config : load_config();
}

void write_config(const mutable_config_t* new_config);

#endif
