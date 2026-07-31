/******************************************************************************/
/*  SPDX-License-Identifier: MIT                                              */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku                         */
/*  SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com) */
/*  https://github.com/sugoku/piuio-pico-brokeIO                              */
/******************************************************************************/

#ifndef _KEYBOARD_REPORT_H
#define _KEYBOARD_REPORT_H

#include "../descriptors/keyboard_desc.h"
#include "../piuio_structs.h"
#include "../piuio_config.h"

/// Standard HID Boot Protocol Keyboard Report.
typedef struct TU_ATTR_PACKED
{
    uint8_t keycode[16]; /**< Key codes of the currently pressed keys. */
} KeyboardReport;

static KeyboardReport keyboardReport = {
    .keycode = { 0 }
};

void keyboard_press_key(uint8_t code) {
    if (code >= HID_KEY_CONTROL_LEFT) {
        keyboardReport.keycode[0] |= KEYBOARD_MODIFIER_LEFTCTRL;
    } else if ((code >> 3) < KEY_COUNT - 2) {
        keyboardReport.keycode[(code >> 3) + 1] |= 1 << (code & 7);
    }
}

void keyboard_release_all_keys(void) {
    for (uint8_t i = 0; i < (sizeof(keyboardReport.keycode) / sizeof(keyboardReport.keycode[0])); i++) {
        keyboardReport.keycode[i] = 0;
    }
}

uint16_t keyboard_get_report(KeyboardReport** report, struct inputArray* input) {
    keyboard_release_all_keys();

    if(!input->p1_ul)  { keyboard_press_key(KEYCODE_P1_UPLEFT); }
    if(!input->p1_ur)  { keyboard_press_key(KEYCODE_P1_UPRIGHT); }
    if(!input->p1_cn)  { keyboard_press_key(KEYCODE_P1_CENTER); }
    if(!input->p1_dl)  { keyboard_press_key(KEYCODE_P1_DOWNLEFT); }
    if(!input->p1_dr)  { keyboard_press_key(KEYCODE_P1_DOWNRIGHT); }
    if(!input->p1_select)  { keyboard_press_key(KEYCODE_P1_MENU_SELECT); }
    if(!input->p1_menu_l)  { keyboard_press_key(KEYCODE_P1_MENU_LEFT); }
    if(!input->p1_menu_r)  { keyboard_press_key(KEYCODE_P1_MENU_RIGHT); }

    if(!input->p2_ul)  { keyboard_press_key(KEYCODE_P2_UPLEFT); }
    if(!input->p2_ur)  { keyboard_press_key(KEYCODE_P2_UPRIGHT); }
    if(!input->p2_cn)  { keyboard_press_key(KEYCODE_P2_CENTER); }
    if(!input->p2_dl)  { keyboard_press_key(KEYCODE_P2_DOWNLEFT); }
    if(!input->p2_dr)  { keyboard_press_key(KEYCODE_P2_DOWNRIGHT); }
    if(!input->p2_select)  { keyboard_press_key(KEYCODE_P2_MENU_SELECT); }
    if(!input->p2_menu_l)  { keyboard_press_key(KEYCODE_P2_MENU_LEFT); }
    if(!input->p2_menu_r)  { keyboard_press_key(KEYCODE_P2_MENU_RIGHT); }

    if(!input->p1_coin)  { keyboard_press_key(KEYCODE_P1_COIN); }
    if(!input->p2_coin)  { keyboard_press_key(KEYCODE_P2_COIN); }

    if(!input->test)     { keyboard_press_key(KEYCODE_TEST); }
    if(!input->service)  { keyboard_press_key(KEYCODE_SERVICE); }
    if(!input->clear)    { keyboard_press_key(KEYCODE_CLEAR); }

    *report = &keyboardReport;
    return sizeof(KeyboardReport);
}

#endif