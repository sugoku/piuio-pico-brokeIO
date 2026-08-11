
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/clocks.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#include "pio_usb.h"
#include "tusb.h"
#include "host_hid.h"

#include "input_mux4067.h"

#define C_DP 16
#define C_DM 17 // must be C_DP + 1

static uint8_t const keycode2ascii[128][2] =  { HID_KEYCODE_TO_ASCII };

// Button state handed from core1 (USB host stack) to core0 (main loop).
// Core1 is the sole writer and stores the whole word at once; core0 is the
// sole reader. That keeps mux4067_vals single-writer on core0 and avoids a
// cross-core read-modify-write on the global mux word.
static volatile uint32_t hid_buttons = 0;

uint32_t host_hid_buttons(void) {
  return hid_buttons;
}

/*------------- MAIN -------------*/

// core1: handle host events
void core1_main() {
  sleep_ms(10);

  // Use tuh_configure() to pass pio configuration to the host stack
  // Note: tuh_configure() must be called before
  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = C_DP;
  tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

  // To run USB SOF interrupt in core1, init host stack for pio_usb (roothub
  // port1) on core1
  tuh_init(1);

  while (true) {
    tuh_task(); // tinyusb host task
  }
}

/**
 * Start the PIO USB host stack on core1.
 *
 * clk_sys must already be a multiple of 12MHz (set at the top of main()) —
 * Pico-PIO-USB derives its bit timing from it and will silently mis-sample
 * the bus otherwise, so fail loudly at boot rather than intermittently later.
 */
void host_hid_init() {
  hard_assert(clock_get_hz(clk_sys) % 12000000 == 0);

  multicore_reset_core1();
  // all USB task run in core1
  multicore_launch_core1(core1_main);
}

//--------------------------------------------------------------------+
// Host HID
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  (void)desc_report;
  (void)desc_len;

  // Interface protocol (hid_interface_protocol_enum_t)
  const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

//   char tempbuf[256];
//   int count = sprintf(tempbuf, "[%04x:%04x][%u] HID Interface%u, Protocol = %s\r\n", vid, pid, dev_addr, instance, protocol_str[itf_protocol]);

  // Receive report from boot keyboard & mouse only
  // tuh_hid_report_received_cb() will be invoked when report is available
  if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD || itf_protocol == HID_ITF_PROTOCOL_MOUSE || itf_protocol == HID_ITF_PROTOCOL_NONE)
  {
    if ( !tuh_hid_receive_report(dev_addr, instance) )
    {
    //   tud_cdc_write_str("Error: cannot request report\r\n");
    }
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
//   char tempbuf[256];
//   int count = sprintf(tempbuf, "[%u] HID Interface%u is unmounted\r\n", dev_addr, instance);
//   tud_cdc_write(tempbuf, count);
//   tud_cdc_write_flush();
}

// look up new key in previous keys
static inline bool find_key_in_report(hid_keyboard_report_t const *report, uint8_t search)
{
  for(uint8_t i=0; i<6; i++)
  {
    uint8_t keycode = report->keycode[i];

    // keycodes are 8 bit but the ascii table only covers 0x00-0x7F. Media keys
    // and garbled reports go above that and would read past the end of the
    // table, returning flash bytes that can happen to match `search` and fire a
    // phantom test/service/clear.
    if (keycode >= TU_ARRAY_SIZE(keycode2ascii)) continue;

    if (keycode2ascii[keycode][0] == search)  return true;
  }

  return false;
}


/** Map the keyboard report onto test/service/clear and publish it to core0. */
static void process_kbd_report(uint8_t dev_addr, hid_keyboard_report_t const *report)
{
  (void) dev_addr;

  // assemble the full word locally, then publish it in one store so core0
  // cannot observe test updated but service/clear not yet applied
  uint32_t buttons = 0;

  SETORCLRBIT(buttons, MUX4067_TEST, find_key_in_report(report, 'a'));
  SETORCLRBIT(buttons, MUX4067_SERVICE, find_key_in_report(report, 'b'));
  SETORCLRBIT(buttons, MUX4067_CLEAR, find_key_in_report(report, 'c'));

  __dmb();
  hid_buttons = buttons;
}

// send mouse report to usb device CDC
static void process_mouse_report(uint8_t dev_addr, hid_mouse_report_t const * report)
{
  //------------- button state  -------------//
  //uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
//   char l = report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-';
//   char m = report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-';
//   char r = report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-';

//   char tempbuf[32];
//   int count = sprintf(tempbuf, "[%u] %c%c%c %d %d %d\r\n", dev_addr, l, m, r, report->x, report->y, report->wheel);

//   tud_cdc_write(tempbuf, count);
//   tud_cdc_write_flush();
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  // a short or truncated report would be read past its end once cast
  switch(itf_protocol)
  {
    case HID_ITF_PROTOCOL_KEYBOARD:
      if (len >= sizeof(hid_keyboard_report_t))
        process_kbd_report(dev_addr, (hid_keyboard_report_t const*) report );
    break;

    case HID_ITF_PROTOCOL_MOUSE:
      if (len >= sizeof(hid_mouse_report_t))
        process_mouse_report(dev_addr, (hid_mouse_report_t const*) report );
    break;

    default: 
        // gamepad?
    break;
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    // tud_cdc_write_str("Error: cannot request report\r\n");
  }
}