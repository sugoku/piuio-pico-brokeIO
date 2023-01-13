/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

#ifndef PIUIO_PICO_PIUIO_STRUCTS_H
#define PIUIO_PICO_PIUIO_STRUCTS_H

enum state {
    PLAYER_1,
    CABINET,
    PLAYER_2,
    UNUSED
};

struct inputArray {
    union{
        uint8_t data[8];
        struct {
            // p1 input byte 0
            uint8_t p1_ul : 1;
            uint8_t p1_ur : 1;
            uint8_t p1_cn : 1;
            uint8_t p1_dl : 1;
            uint8_t p1_dr : 1;
            uint8_t empty1 : 3;

            // p1 input byte 1
            uint8_t empty2 : 1;
            uint8_t test : 1;
            uint8_t p1_coin : 1;
            uint8_t empty3 : 3;
            uint8_t service : 1;
            uint8_t clear : 1;

            // p2 input byte 2
            uint8_t p2_ul : 1;
            uint8_t p2_ur : 1;
            uint8_t p2_cn : 1;
            uint8_t p2_dl : 1;
            uint8_t p2_dr : 1;
            uint8_t empty4 : 3;

            // p2 input byte 3
            uint8_t empty5 : 2;
            uint8_t p2_coin : 1;
            uint8_t empty6 : 5;

            uint32_t empty7 : 32;
        };
    };
};

struct lightsArray {
    union{
        uint8_t data[8];
        struct {
            // p1 cmd byte 0
            uint8_t p1_mux : 2;
            uint8_t p1_ul_light : 1;
            uint8_t p1_ur_light : 1;
            uint8_t p1_cn_light : 1;
            uint8_t p1_dl_light : 1;
            uint8_t p1_dr_light : 1;
            uint8_t empty1 : 1;

            // p1 cmd byte 1
            uint8_t empty2 : 2;
            uint8_t bass_light : 1;
            uint8_t empty3 : 5;

            // p2 cmd byte 2
            uint8_t p2_mux : 2;
            uint8_t p2_ul_light : 1;
            uint8_t p2_ur_light : 1;
            uint8_t p2_cn_light : 1;
            uint8_t p2_dl_light : 1;
            uint8_t p2_dr_light : 1;
            uint8_t r2_halo : 1;

            // p2 cmd byte 3
            uint8_t r1_halo : 1;
            uint8_t l2_halo : 1;
            uint8_t l1_halo : 1;
            uint8_t empty5 : 3;
            uint8_t r1_halo_dupe : 1;
            uint8_t r2_halo_dupe : 1;

            uint32_t empty6 : 32;
        };
    };
};

#endif //PIUIO_PICO_PIUIO_STRUCTS_H
