/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  SPDX-FileCopyrightText: Copyright (c) 2023 sugoku     */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#ifndef _LXIO_REPORT_H
#define _LXIO_REPORT_H

typedef struct __attribute((packed, aligned(1))) LXIOInputReport {
    union{
        uint8_t data[16];
        struct {
            // input byte 0
            uint8_t p1_ul_mux0 : 1;
            uint8_t p1_ur_mux0 : 1;
            uint8_t p1_cn_mux0 : 1;
            uint8_t p1_dl_mux0 : 1;
            uint8_t p1_dr_mux0 : 1;
            uint8_t p1_select_mux0 : 1;
            uint8_t p1_menu_l_mux0 : 1;
            uint8_t p1_menu_r_mux0 : 1;

            // input byte 1
            uint8_t p1_ul_mux1 : 1;
            uint8_t p1_ur_mux1 : 1;
            uint8_t p1_cn_mux1 : 1;
            uint8_t p1_dl_mux1 : 1;
            uint8_t p1_dr_mux1 : 1;
            uint8_t p1_select_mux1 : 1;
            uint8_t p1_menu_l_mux1 : 1;
            uint8_t p1_menu_r_mux1 : 1;

            // input byte 2
            uint8_t p1_ul_mux2 : 1;
            uint8_t p1_ur_mux2 : 1;
            uint8_t p1_cn_mux2 : 1;
            uint8_t p1_dl_mux2 : 1;
            uint8_t p1_dr_mux2 : 1;
            uint8_t p1_select_mux2 : 1;
            uint8_t p1_menu_l_mux2 : 1;
            uint8_t p1_menu_r_mux2 : 1;

            // input byte 3
            uint8_t p1_ul_mux3 : 1;
            uint8_t p1_ur_mux3 : 1;
            uint8_t p1_cn_mux3 : 1;
            uint8_t p1_dl_mux3 : 1;
            uint8_t p1_dr_mux3 : 1;
            uint8_t p1_select_mux3 : 1;
            uint8_t p1_menu_l_mux3 : 1;
            uint8_t p1_menu_r_mux3 : 1;

            // input byte 4
            uint8_t p2_ul_mux0 : 1;
            uint8_t p2_ur_mux0 : 1;
            uint8_t p2_cn_mux0 : 1;
            uint8_t p2_dl_mux0 : 1;
            uint8_t p2_dr_mux0 : 1;
            uint8_t p2_select_mux0 : 1;
            uint8_t p2_menu_l_mux0 : 1;
            uint8_t p2_menu_r_mux0 : 1;

            // input byte 5
            uint8_t p2_ul_mux1 : 1;
            uint8_t p2_ur_mux1 : 1;
            uint8_t p2_cn_mux1 : 1;
            uint8_t p2_dl_mux1 : 1;
            uint8_t p2_dr_mux1 : 1;
            uint8_t p2_select_mux1 : 1;
            uint8_t p2_menu_l_mux1 : 1;
            uint8_t p2_menu_r_mux1 : 1;

            // input byte 6
            uint8_t p2_ul_mux2 : 1;
            uint8_t p2_ur_mux2 : 1;
            uint8_t p2_cn_mux2 : 1;
            uint8_t p2_dl_mux2 : 1;
            uint8_t p2_dr_mux2 : 1;
            uint8_t p2_select_mux2 : 1;
            uint8_t p2_menu_l_mux2 : 1;
            uint8_t p2_menu_r_mux2 : 1;

            // input byte 7
            uint8_t p2_ul_mux3 : 1;
            uint8_t p2_ur_mux3 : 1;
            uint8_t p2_cn_mux3 : 1;
            uint8_t p2_dl_mux3 : 1;
            uint8_t p2_dr_mux3 : 1;
            uint8_t p2_select_mux3 : 1;
            uint8_t p2_menu_l_mux3 : 1;
            uint8_t p2_menu_r_mux3 : 1;

            // input byte 8
            uint8_t empty3_1: 1;
            uint8_t test1: 1;
            uint8_t p1_coin: 1;
            uint8_t empty4_1: 3;
            uint8_t service1: 1;
            uint8_t clear1: 1;

            // input byte 9
            uint8_t empty3_2: 1;
            uint8_t test2: 1;
            uint8_t p2_coin: 1;
            uint8_t empty4_2: 3;
            uint8_t service2: 1;
            uint8_t clear2: 1;

            // input byte 10
            uint8_t p1_ul_menu : 1;
            uint8_t p1_ur_menu : 1;
            uint8_t p1_cn_menu : 1;
            uint8_t p1_dl_menu : 1;
            uint8_t p1_dr_menu : 1;
            uint8_t empty5_1 : 3;

            // input byte 11
            uint8_t p2_ul_menu : 1;
            uint8_t p2_ur_menu : 1;
            uint8_t p2_cn_menu : 1;
            uint8_t p2_dl_menu : 1;
            uint8_t p2_dr_menu : 1;
            uint8_t empty5_2 : 3;

            // input bytes 12-15
            uint32_t empty6 : 32;
        };
    };
} LXIOInputReport_t;

typedef struct __attribute((packed, aligned(1))) LXIOOutputReport {
    union{
        uint8_t data[16];
        struct {
            // output byte 0
            uint8_t empty1 : 2;
            uint8_t p1_ul_light : 1;
            uint8_t p1_ur_light : 1;
            uint8_t p1_cn_light : 1;
            uint8_t p1_dl_light : 1;
            uint8_t p1_dr_light : 1;
            uint8_t empty2 : 1;

            // output byte 1
            uint8_t empty3 : 2;
            uint8_t bass_light : 1;
            uint8_t empty4 : 5;

            // output byte 2
            uint8_t empty5 : 2;
            uint8_t p2_ul_light : 1;
            uint8_t p2_ur_light : 1;
            uint8_t p2_cn_light : 1;
            uint8_t p2_dl_light : 1;
            uint8_t p2_dr_light : 1;
            uint8_t r2_halo : 1;

            // output byte 3
            uint8_t r1_halo : 1;
            uint8_t l2_halo : 1;
            uint8_t l1_halo : 1;
            uint8_t p1_coin_cnt : 1;
            uint8_t p2_coin_cnt : 1;
            uint8_t usb_maybe : 1;
            uint8_t r1_halo_dupe : 1;
            uint8_t r2_halo_dupe : 1;

            // output byte 4
            uint8_t p1_ul_light_menu : 1;
            uint8_t p1_ur_light_menu : 1;
            uint8_t p1_cn_light_menu : 1;
            uint8_t p1_dl_light_menu : 1;
            uint8_t p1_dr_light_menu : 1;
            uint8_t empty6_1 : 3;

            // output byte 5
            uint8_t p2_ul_light_menu : 1;
            uint8_t p2_ur_light_menu : 1;
            uint8_t p2_cn_light_menu : 1;
            uint8_t p2_dl_light_menu : 1;
            uint8_t p2_dr_light_menu : 1;
            uint8_t empty6_2 : 3;

            // bytes 6-8
            uint32_t empty7 : 24;
            // bytes 9-12
            uint32_t empty8 : 32;
            // bytes 13-16
            uint32_t empty9 : 32;
        };
    };
} LXIOOutputReport_t;

LXIOInputReport_t lxioInputReport = {
    .data = {0xFF}
};

LXIOOutputReport_t lxioOutputReport = {
    .data = {0xFF}
};

uint16_t lxio_get_report(uint8_t** report, struct inputArray* input, struct inputArray* input_mux) {
    memset(lxioInputReport.data, 0xFF, 16);

    lxioInputReport.p1_dl_mux0 = input_mux[0].p1_dl;
    lxioInputReport.p1_ul_mux0 = input_mux[0].p1_ul;
    lxioInputReport.p1_cn_mux0 = input_mux[0].p1_cn;
    lxioInputReport.p1_ur_mux0 = input_mux[0].p1_ur;
    lxioInputReport.p1_dr_mux0 = input_mux[0].p1_dr;
    lxioInputReport.p1_select_mux0 = input->p1_select;
    lxioInputReport.p1_menu_l_mux0 = input->p1_menu_l;
    lxioInputReport.p1_menu_r_mux0 = input->p1_menu_r;

    lxioInputReport.p1_dl_mux1 = input_mux[1].p1_dl;
    lxioInputReport.p1_ul_mux1 = input_mux[1].p1_ul;
    lxioInputReport.p1_cn_mux1 = input_mux[1].p1_cn;
    lxioInputReport.p1_ur_mux1 = input_mux[1].p1_ur;
    lxioInputReport.p1_dr_mux1 = input_mux[1].p1_dr;
    lxioInputReport.p1_select_mux1 = input->p1_select;
    lxioInputReport.p1_menu_l_mux1 = input->p1_menu_l;
    lxioInputReport.p1_menu_r_mux1 = input->p1_menu_r;

    lxioInputReport.p1_dl_mux2 = input_mux[2].p1_dl;
    lxioInputReport.p1_ul_mux2 = input_mux[2].p1_ul;
    lxioInputReport.p1_cn_mux2 = input_mux[2].p1_cn;
    lxioInputReport.p1_ur_mux2 = input_mux[2].p1_ur;
    lxioInputReport.p1_dr_mux2 = input_mux[2].p1_dr;
    lxioInputReport.p1_select_mux2 = input->p1_select;
    lxioInputReport.p1_menu_l_mux2 = input->p1_menu_l;
    lxioInputReport.p1_menu_r_mux2 = input->p1_menu_r;

    lxioInputReport.p1_dl_mux3 = input_mux[3].p1_dl;
    lxioInputReport.p1_ul_mux3 = input_mux[3].p1_ul;
    lxioInputReport.p1_cn_mux3 = input_mux[3].p1_cn;
    lxioInputReport.p1_ur_mux3 = input_mux[3].p1_ur;
    lxioInputReport.p1_dr_mux3 = input_mux[3].p1_dr;
    lxioInputReport.p1_select_mux3 = input->p1_select;
    lxioInputReport.p1_menu_l_mux3 = input->p1_menu_l;
    lxioInputReport.p1_menu_r_mux3 = input->p1_menu_r;

    lxioInputReport.p2_dl_mux0 = input_mux[0].p2_dl;
    lxioInputReport.p2_ul_mux0 = input_mux[0].p2_ul;
    lxioInputReport.p2_cn_mux0 = input_mux[0].p2_cn;
    lxioInputReport.p2_ur_mux0 = input_mux[0].p2_ur;
    lxioInputReport.p2_dr_mux0 = input_mux[0].p2_dr;
    lxioInputReport.p2_select_mux0 = input->p2_select;
    lxioInputReport.p2_menu_l_mux0 = input->p2_menu_l;
    lxioInputReport.p2_menu_r_mux0 = input->p2_menu_r;

    lxioInputReport.p2_dl_mux1 = input_mux[1].p2_dl;
    lxioInputReport.p2_ul_mux1 = input_mux[1].p2_ul;
    lxioInputReport.p2_cn_mux1 = input_mux[1].p2_cn;
    lxioInputReport.p2_ur_mux1 = input_mux[1].p2_ur;
    lxioInputReport.p2_dr_mux1 = input_mux[1].p2_dr;
    lxioInputReport.p2_select_mux1 = input->p2_select;
    lxioInputReport.p2_menu_l_mux1 = input->p2_menu_l;
    lxioInputReport.p2_menu_r_mux1 = input->p2_menu_r;

    lxioInputReport.p2_dl_mux2 = input_mux[2].p2_dl;
    lxioInputReport.p2_ul_mux2 = input_mux[2].p2_ul;
    lxioInputReport.p2_cn_mux2 = input_mux[2].p2_cn;
    lxioInputReport.p2_ur_mux2 = input_mux[2].p2_ur;
    lxioInputReport.p2_dr_mux2 = input_mux[2].p2_dr;
    lxioInputReport.p2_select_mux2 = input->p2_select;
    lxioInputReport.p2_menu_l_mux2 = input->p2_menu_l;
    lxioInputReport.p2_menu_r_mux2 = input->p2_menu_r;

    lxioInputReport.p2_dl_mux3 = input_mux[3].p2_dl;
    lxioInputReport.p2_ul_mux3 = input_mux[3].p2_ul;
    lxioInputReport.p2_cn_mux3 = input_mux[3].p2_cn;
    lxioInputReport.p2_ur_mux3 = input_mux[3].p2_ur;
    lxioInputReport.p2_dr_mux3 = input_mux[3].p2_dr;
    lxioInputReport.p2_select_mux3 = input->p2_select;
    lxioInputReport.p2_menu_l_mux3 = input->p2_menu_l;
    lxioInputReport.p2_menu_r_mux3 = input->p2_menu_r;

    lxioInputReport.p1_coin = input->p1_coin;
    lxioInputReport.p2_coin = input->p2_coin;
    lxioInputReport.test1 = input->test;
    lxioInputReport.service1 = input->service;
    lxioInputReport.clear1 = input->clear;

    *report = lxioInputReport.data;
    return sizeof(LXIOInputReport_t);
}

void lxio_set_report(uint8_t const* buffer, uint16_t bufsize, struct lightsArray* lights) {
    bufsize = (bufsize > 16) ? 16 : bufsize;
    memcpy(lxioOutputReport.data, buffer, bufsize);

    lights->p1_ul_light = lxioOutputReport.p1_ul_light;
    lights->p1_ur_light = lxioOutputReport.p1_ur_light;
    lights->p1_cn_light = lxioOutputReport.p1_cn_light;
    lights->p1_dl_light = lxioOutputReport.p1_dl_light;
    lights->p1_dr_light = lxioOutputReport.p1_dr_light;

    lights->p2_ul_light = lxioOutputReport.p2_ul_light;
    lights->p2_ur_light = lxioOutputReport.p2_ur_light;
    lights->p2_cn_light = lxioOutputReport.p2_cn_light;
    lights->p2_dl_light = lxioOutputReport.p2_dl_light;
    lights->p2_dr_light = lxioOutputReport.p2_dr_light;

    lights->l1_halo = lxioOutputReport.l1_halo;
    lights->l2_halo = lxioOutputReport.l2_halo;
    lights->r1_halo = lxioOutputReport.r1_halo;
    lights->r2_halo = lxioOutputReport.r2_halo;
    lights->bass_light = lxioOutputReport.bass_light;
}

#endif