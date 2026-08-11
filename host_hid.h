#ifndef _HOST_HID_H
#define _HOST_HID_H

#include <stdint.h>

void host_hid_init();

/**
 * Snapshot the button state published by the USB host stack running on core1.
 *
 * Returns a mask in MUX4067_* bit positions, active high. Core1 is the only
 * writer and publishes the whole word in a single aligned 32-bit store, so
 * core0 can read it without a lock and never sees a half-applied key set.
 */
uint32_t host_hid_buttons(void);

#endif
