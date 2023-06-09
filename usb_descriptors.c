/**********************************************************/
/*  SPDX-License-Identifier: MIT                          */
/*  https://github.com/sugoku/piuio-pico-brokeIO          */
/**********************************************************/

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 sugoku
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 * SPDX-FileCopyrightText: Copyright (c) 2019 Ha Thach (tinyusb.org)
 */

#include "tusb.h"
#include "usb_descriptors.h"
#include "input_mode.h"

#include "descriptors/hid_desc.h"
#include "descriptors/keyboard_desc.h"
#include "descriptors/lxio_desc.h"
#include "descriptors/piuio_desc.h"
#include "descriptors/switch_desc.h"
#include "descriptors/xinput_desc.h"



/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]       MIDI | HID | MSC | CDC          [LSB]
 */
// #define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
// #define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
//                            _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) )

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void)
{
	switch (get_input_mode())
	{
        case INPUT_MODE_PIUIO:
            return (uint8_t const*)&piuio_device_descriptor;

		case INPUT_MODE_GAMEPAD:
			return hid_device_descriptor;

        case INPUT_MODE_LXIO:
			return (uint8_t const*)&lxio_device_descriptor;

        case INPUT_MODE_KEYBOARD:
			return keyboard_device_descriptor;

		case INPUT_MODE_XINPUT:
			return xinput_device_descriptor;

		case INPUT_MODE_SWITCH:
			return switch_device_descriptor;

		default:
			return (uint8_t const*)&piuio_device_descriptor;
	}
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
	(void) itf;
	switch (get_input_mode())
	{
        case INPUT_MODE_GAMEPAD:
			return hid_report_descriptor;

        case INPUT_MODE_LXIO:
			return lxio_report_descriptor;

		case INPUT_MODE_KEYBOARD:
			return keyboard_report_descriptor;

        case INPUT_MODE_SWITCH:
			return switch_report_descriptor;

		default:
			return hid_report_descriptor;
	}
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
	switch (get_input_mode())
	{
        case INPUT_MODE_GAMEPAD:
			return hid_configuration_descriptor;

        case INPUT_MODE_LXIO:
			return lxio_configuration_descriptor;

        case INPUT_MODE_KEYBOARD:
			return keyboard_configuration_descriptor;

		case INPUT_MODE_XINPUT:
			return xinput_configuration_descriptor;

		case INPUT_MODE_SWITCH:
			return switch_configuration_descriptor;

		default:
			return piuio_configuration_descriptor;
	}
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* brokeio_string_desc_arr [] =
        {
                (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
                "sugoku",                     // 1: Manufacturer
                "brokeIO",              // 2: Product
                "727",                      // 3: Serials, should use chip ID
                "PIUIO-pico"               // 4: Vendor Interface
        };

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
// based on GP2040-CE implementation
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void) langid;

	uint16_t size;
	uint8_t charCount;

    const char* string_desc;

    switch (get_input_mode())
	{
        case INPUT_MODE_GAMEPAD:
			string_desc = (const char*)hid_string_descriptors;
            break;

        case INPUT_MODE_LXIO:
			string_desc = (const char*)lxio_string_descriptors;
            break;

        case INPUT_MODE_KEYBOARD:
			string_desc = (const char*)keyboard_string_descriptors;
            break;

		case INPUT_MODE_XINPUT:
			string_desc = (const char*)xinput_string_descriptors;
            break;

		case INPUT_MODE_SWITCH:
			string_desc = (const char*)switch_string_descriptors;
            break;

		default:
			string_desc = (const char*)piuio_string_descriptors;
            break;
	}

    // Cap at max char
	charCount = strlen(string_desc);
	if (charCount > 31)
		charCount = 31;

	for (uint8_t i = 0; i < charCount; i++)
		_desc_str[1 + i] = string_desc[i];

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (2 * charCount + 2);
	_desc_str[1] = 0x03;

	return _desc_str;
}