/******************************************************************************/
/*  SPDX-License-Identifier: MIT                                              */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku                         */
/*  SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com) */
/*  https://github.com/sugoku/piuio-pico-brokeIO                              */
/******************************************************************************/

#ifndef _HID_REPORT_H
#define _HID_REPORT_H

#include "../piuio_structs.h"

#define HID_ENDPOINT_SIZE 64

// HAT report (4 bits)
#define HID_HAT_UP        0x00
#define HID_HAT_UPRIGHT   0x01
#define HID_HAT_RIGHT     0x02
#define HID_HAT_DOWNRIGHT 0x03
#define HID_HAT_DOWN      0x04
#define HID_HAT_DOWNLEFT  0x05
#define HID_HAT_LEFT      0x06
#define HID_HAT_UPLEFT    0x07
#define HID_HAT_NOTHING   0x08

// Button report (16 bits)
#define HID_MASK_SQUARE   (1U <<  0)
#define HID_MASK_CROSS    (1U <<  1)
#define HID_MASK_CIRCLE   (1U <<  2)
#define HID_MASK_TRIANGLE (1U <<  3)
#define HID_MASK_L1       (1U <<  4)
#define HID_MASK_R1       (1U <<  5)
#define HID_MASK_L2       (1U <<  6)
#define HID_MASK_R2       (1U <<  7)
#define HID_MASK_SELECT   (1U <<  8)
#define HID_MASK_START    (1U <<  9)
#define HID_MASK_L3       (1U << 10)
#define HID_MASK_R3       (1U << 11)
#define HID_MASK_PS       (1U << 12)
#define HID_MASK_TP       (1U << 13)

// Switch analog sticks only report 8 bits
#define HID_JOYSTICK_MIN 0x00
#define HID_JOYSTICK_MID 0x80
#define HID_JOYSTICK_MAX 0xFF

typedef struct __attribute((packed, aligned(1)))
{
    // digital buttons, 0 = off, 1 = on

    uint8_t p1_ul : 1;
    uint8_t p1_ur : 1;
    uint8_t p1_cn : 1;
    uint8_t p1_dl : 1;
    uint8_t p1_dr : 1;
    uint8_t p1_select : 1;
    uint8_t p1_menu_l : 1;
    uint8_t p1_menu_r : 1;

    uint8_t p2_ul : 1;
    uint8_t p2_ur : 1;
    uint8_t p2_cn : 1;
    uint8_t p2_dl : 1;
    uint8_t p2_dr : 1;
    uint8_t p2_select : 1;
    uint8_t p2_menu_l : 1;
    uint8_t p2_menu_r : 1;

    uint8_t p1_coin : 1;
    uint8_t p2_coin : 1;
    
    uint8_t test : 1;
    uint8_t service : 1;
    uint8_t clear : 1;

    uint8_t padding : 3;

    // digital direction, use the dir_* constants(enum)
    // 8 = center, 0 = up, 1 = up/right, 2 = right, 3 = right/down
    // 4 = down, 5 = down/left, 6 = left, 7 = left/up

    uint8_t direction;

    // left and right analog sticks, 0x00 left/up, 0x80 middle, 0xff right/down

    uint8_t l_x_axis;
    uint8_t l_y_axis;
    uint8_t r_x_axis;
    uint8_t r_y_axis;

    // Gonna assume these are button analog values for the d-pad.
    // NOTE: NOT EVEN SURE THIS IS RIGHT, OR IN THE CORRECT ORDER
    // uint8_t right_axis;
    // uint8_t left_axis;
    // uint8_t up_axis;
    // uint8_t down_axis;

    // // button axis, 0x00 = unpressed, 0xff = fully pressed

    // uint8_t triangle_axis;
    // uint8_t circle_axis;
    // uint8_t cross_axis;
    // uint8_t square_axis;

    // uint8_t l1_axis;
    // uint8_t r1_axis;
    // uint8_t l2_axis;
    // uint8_t r2_axis;
} HIDReport;

static HIDReport hidReport = {
    .p1_ul = 0, .p1_ur = 0, .p1_cn = 0, .p1_dl = 0, .p1_dr = 0, .p1_select = 0, .p1_menu_l = 0, .p1_menu_r = 0,
    .p2_ul = 0, .p2_ur = 0, .p2_cn = 0, .p2_dl = 0, .p2_dr = 0, .p2_select = 0, .p2_menu_l = 0, .p2_menu_r = 0,
    .p1_coin = 0, .p2_coin = 0, .test = 0, .service = 0, .clear = 0, .padding = 0,
    .direction = 0x08,
    .l_x_axis = 0x80, .l_y_axis = 0x80, .r_x_axis = 0x80, .r_y_axis = 0x80,
    // .right_axis = 0x00, .left_axis = 0x00, .up_axis = 0x00, .down_axis = 0x00,
    // .triangle_axis = 0x00, .circle_axis = 0x00, .cross_axis = 0x00, .square_axis = 0x00,
    // .l1_axis = 0x00, .r1_axis = 0x00, .l2_axis = 0x00, .r2_axis = 0x00
};

uint16_t hid_get_report(HIDReport** report, struct inputArray* input) {
    // switch (state.dpad & GAMEPAD_MASK_DPAD)
    // {
    //     case GAMEPAD_MASK_UP:                        hidReport.direction = HID_HAT_UP;        break;
    //     case GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT:   hidReport.direction = HID_HAT_UPRIGHT;   break;
    //     case GAMEPAD_MASK_RIGHT:                     hidReport.direction = HID_HAT_RIGHT;     break;
    //     case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_RIGHT: hidReport.direction = HID_HAT_DOWNRIGHT; break;
    //     case GAMEPAD_MASK_DOWN:                      hidReport.direction = HID_HAT_DOWN;      break;
    //     case GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT:  hidReport.direction = HID_HAT_DOWNLEFT;  break;
    //     case GAMEPAD_MASK_LEFT:                      hidReport.direction = HID_HAT_LEFT;      break;
    //     case GAMEPAD_MASK_UP | GAMEPAD_MASK_LEFT:    hidReport.direction = HID_HAT_UPLEFT;    break;
    //     default:                                     hidReport.direction = HID_HAT_NOTHING;   break;
    // }
    hidReport.p1_ul = !input->p1_ul;
    hidReport.p1_ur = !input->p1_ur;
    hidReport.p1_cn = !input->p1_cn;
    hidReport.p1_dl = !input->p1_dl;
    hidReport.p1_dr = !input->p1_dr;
    hidReport.p1_select = !input->p1_select;
    hidReport.p1_menu_l = !input->p1_menu_l;
    hidReport.p1_menu_r = !input->p1_menu_r;
    
    hidReport.p2_ul = !input->p2_ul;
    hidReport.p2_ur = !input->p2_ur;
    hidReport.p2_cn = !input->p2_cn;
    hidReport.p2_dl = !input->p2_dl;
    hidReport.p2_dr = !input->p2_dr;
    hidReport.p2_select = !input->p2_select;
    hidReport.p2_menu_l = !input->p2_menu_l;
    hidReport.p2_menu_r = !input->p2_menu_r;

    hidReport.p1_coin = !input->p1_coin;
    hidReport.p2_coin = !input->p2_coin;

    hidReport.test = !input->test;
    hidReport.service = !input->service;
    hidReport.clear = !input->clear;

    hidReport.direction = HID_HAT_NOTHING;

    hidReport.l_x_axis = 0x80; hidReport.l_y_axis = 0x80; hidReport.r_x_axis = 0x80; hidReport.r_y_axis = 0x80;
    // hidReport.right_axis = 0x00; hidReport.left_axis = 0x00; hidReport.up_axis = 0x00; hidReport.down_axis = 0x00;
    // hidReport.triangle_axis = 0x00; hidReport.circle_axis = 0x00; hidReport.cross_axis = 0x00; hidReport.square_axis = 0x00;
    // hidReport.l1_axis = 0x00; hidReport.r1_axis = 0x00; hidReport.l2_axis = 0x00; hidReport.r2_axis = 0x00;

    *report = &hidReport;
    return sizeof(HIDReport);
}

#endif