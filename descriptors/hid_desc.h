/******************************************************************************/
/*  SPDX-License-Identifier: MIT                                              */
/*  SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com) */
/*  https://github.com/sugoku/piuio-pico-brokeIO                              */
/******************************************************************************/

#ifndef _HID_DESC_H
#define _HID_DESC_H

#include <stdint.h>
#include "../piuio_config.h"

#define HID_ENDPOINT_SIZE 64

/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE    64

#define GAMEPAD_INTERFACE    0
#define GAMEPAD_ENDPOINT    1
#define GAMEPAD_SIZE        64

static const uint8_t hid_string_language[]     = { 0x09, 0x04 };
static const uint8_t hid_string_manufacturer[] = "sugoku";
static const uint8_t hid_string_product[]      = "brokeIO RP2040 (Gamepad)";
static const uint8_t hid_string_version[]      = "727";

static const uint8_t *hid_string_descriptors[] =
{
    hid_string_language,
    hid_string_manufacturer,
    hid_string_product,
    hid_string_version
};

static const uint8_t hid_device_descriptor[] =
{
        18,                                  // bLength
        1,                                  // bDescriptorType
        0x00, 0x02,                          // bcdUSB 2.00
        0,                                  // bDeviceClass
        0,                                  // bDeviceSubClass
        0,                                  // bDeviceProtocol
        ENDPOINT0_SIZE,                      // bMaxPacketSize0
        LSB(VENDOR_ID), MSB(VENDOR_ID),      // idVendor
        LSB(PRODUCT_ID_GAMEPAD), MSB(PRODUCT_ID_GAMEPAD), // idProduct
        0x00, 0x01,                          // bcdDevice
        1,                                  // iManufacturer
        2,                                  // iProduct
        0,                                  // iSerialNumber
        1                                  // bNumConfigurations
};

static const uint8_t hid_report_descriptor[] =
{
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,        // USAGE (Gamepad)
    0xa1, 0x01,        // COLLECTION (Application)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x35, 0x00,        //   PHYSICAL_MINIMUM (0)
    0x45, 0x01,        //   PHYSICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x0f,        //   REPORT_COUNT (15)
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
    0x29, 0x0f,        //   USAGE_MAXIMUM (Button 15)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x01,        //   INPUT (Cnst,Ary,Abs)
    0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
    0x25, 0x07,        //   LOGICAL_MAXIMUM (7)
    0x46, 0x3b, 0x01,  //   PHYSICAL_MAXIMUM (315)
    0x75, 0x04,        //   REPORT_SIZE (4)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x65, 0x14,        //   UNIT (Eng Rot:Angular Pos)
    0x09, 0x39,        //   USAGE (Hat switch)
    0x81, 0x42,        //   INPUT (Data,Var,Abs,Null)
    0x65, 0x00,        //   UNIT (None)
    0x95, 0x01,        //   REPORT_COUNT (1)
    0x81, 0x01,        //   INPUT (Cnst,Ary,Abs)
    0x26, 0xff, 0x00,  //   LOGICAL_MAXIMUM (255)
    0x46, 0xff, 0x00,  //   PHYSICAL_MAXIMUM (255)
    0x09, 0x30,        //   USAGE (X)
    0x09, 0x31,        //   USAGE (Y)
    0x09, 0x32,        //   USAGE (Z)
    0x09, 0x35,        //   USAGE (Rz)
    0x75, 0x08,        //   REPORT_SIZE (8)
    0x95, 0x04,        //   REPORT_COUNT (4)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    // 0x06, 0x00, 0xff,  //   USAGE_PAGE (Vendor Specific)
    // 0x09, 0x20,        //   Unknown
    // 0x09, 0x21,        //   Unknown
    // 0x09, 0x22,        //   Unknown
    // 0x09, 0x23,        //   Unknown
    // 0x09, 0x24,        //   Unknown
    // 0x09, 0x25,        //   Unknown
    // 0x09, 0x26,        //   Unknown
    // 0x09, 0x27,        //   Unknown
    // 0x09, 0x28,        //   Unknown
    // 0x09, 0x29,        //   Unknown
    // 0x09, 0x2a,        //   Unknown
    // 0x09, 0x2b,        //   Unknown
    // 0x95, 0x0c,        //   REPORT_COUNT (12)
    // 0x81, 0x02,        //   INPUT (Data,Var,Abs)
    // 0x0a, 0x21, 0x26,  //   Unknown
    // 0x95, 0x08,        //   REPORT_COUNT (8)
    // 0xb1, 0x02,        //   FEATURE (Data,Var,Abs)
    
    // 16-byte output report for lights
    0x09, 0x00,        //   USAGE (Undefined)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,  //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,        //   REPORT_SIZE (8)
    0x95, 0x10,        //   REPORT_COUNT (16)
    0x91, 0x02,        //   OUTPUT (Data,Var,Abs)

    0xc0               // END_COLLECTION
};

static const uint8_t hid_hid_descriptor[] =
{
        0x09,                                 // bLength
        0x21,                                 // bDescriptorType (HID)
        0x11, 0x01,                             // bcdHID 1.11
        0x00,                                 // bCountryCode
        0x01,                                 // bNumDescriptors
        0x22,                                 // bDescriptorType[0] (HID)
        sizeof(hid_report_descriptor), 0x00, // wDescriptorLength[0] 90
};

#define CONFIG1_DESC_SIZE        (9+9+9+7)
static const uint8_t hid_configuration_descriptor[] =
{
        // configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
    9,                               // bLength;
    2,                               // bDescriptorType;
    LSB(CONFIG1_DESC_SIZE),        // wTotalLength
    MSB(CONFIG1_DESC_SIZE),
    1,                               // bNumInterfaces
    1,                               // bConfigurationValue
    0,                               // iConfiguration
    0x80,                          // bmAttributes
    MAX_USB_POWER,                   // bMaxPower
        // interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
    9,                               // bLength
    4,                               // bDescriptorType
    GAMEPAD_INTERFACE,             // bInterfaceNumber
    0,                               // bAlternateSetting
    1,                               // bNumEndpoints
    0x03,                           // bInterfaceClass (0x03 = HID)
    0x00,                           // bInterfaceSubClass (0x00 = No Boot)
    0x00,                           // bInterfaceProtocol (0x00 = No Protocol)
    0,                               // iInterface
        // HID interface descriptor, HID 1.11 spec, section 6.2.1
    9,                               // bLength
    0x21,                           // bDescriptorType
    0x11, 0x01,                       // bcdHID
    0,                               // bCountryCode
    1,                               // bNumDescriptors
    0x22,                           // bDescriptorType
    sizeof(hid_report_descriptor), // wDescriptorLength
    0,
        // endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
    7,                                // bLength
    5,                               // bDescriptorType
    GAMEPAD_ENDPOINT | 0x80,       // bEndpointAddress
    0x03,                           // bmAttributes (0x03=intr)
    GAMEPAD_SIZE, 0,               // wMaxPacketSize
    1                               // bInterval (1 ms)
};

#endif