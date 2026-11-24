# brokeIO

This is a user guide for the brokeIO, **B**edrock's **r**eplacement for **o**bsolete **K**orean **e**lectronics **IO**.

## Introduction

brokeIO is a drop-in replacement for the PIUIO, which is the I/O board used in Pump It Up machines.

Specifically, it can be used for cabinets with MK6 computers and newer that have a JAMMA PCB.
GX, SXv2, FX, CX, and TX cabinets are the ones that came with the above requirements, though
older machines can be updates to support an MK6 or newer. LX cabinets use a newer I/O solution, commonly called
the LXIO. This board is *not* a replacement for the LXIO.

brokeIO is not just a direct PIUIO replacement, but has several additional features.
The most notable improvements are in the software: brokeIO is capable of emulating several kinds of USB devices, including:

- PIUIO
- Gamepad (DInput)
- LXIO
- Keyboard
- XInput (Xbox 360 Controller)
- Nintendo Switch
- Nintendo GameCube Adapter
- Serial CDC

brokeIO is programmed with built-in debouncing and is very fast, to the point where I had to intentionally slow
it down...so you won't have to worry about slow polling issues due to the hardware.

Currently, options besides mode changes (debouncing times, controller bindings, etc.)
can be configured by recompiling and reflashing the firmware, which can be
uploaded by copying it through a USB flash drive interface.
There is no configuration tool yet, but I am working on one.

In terms of the hardware, brokeIO breaks out the card edge as well as the lights connectors located on the JAMMA PCB,
allowing you to theoretically bypass the JAMMA PCB for a DIY setup.

## Installation

TL;DR if you have a PIUIO, it's a drop-in replacement, but use the ribbon cables included with the brokeIO!

1. Ensure that you have a brokeIO circuit board, two IDC 34-pin ribbon cables, and USB-B cable. Also, ensure that you have a compatible system with:
   1. Motherboard with a free USB port or pin header, as well as a free PCIe slot (only required for mounting)
   2. Power supply with a molex power cable (or some way to supply 12V and 5V DC)
   3. JAMMA circuit board, or some way to connect the brokeIO pinouts to your machine
2. If there is a PIUIO installed in your system, unplug the existing PIUIO circuit board from your motherboard and power supply. This includes the USB-B cable, two IDC cables connecting the PIUIO and JAMMA board, and molex cable.
3. Insert the brokeIO in a free PCIe or PCI slot, preferably as close to the edge of the motherboard as possible; if you can't do this, you will need to find a secure place for it.
*The order is not specific for steps 4-6:*
1. Connect the USB-B (square-shaped) end of your USB cable to the brokeIO. Plug the other end into a USB port or the motherboard header, depending on what type cable you have.
   1. If you have a USB-B to 5-pin motherboard cable, make sure when plugging into the motherboard that the black wire goes to the corner pin that does not have an adjacent pin on the next row.
2. Connect the two IDC cables to the JAMMA. The cables are reversible, but there are notches on the IDC connectors so they can only be inserted in one way.
3. From your power supply, connect molex connectors to your brokeIO and JAMMA PCBs.

**NOTE:** For brokeIO, you must use the IDC cables included with the brokeIO, or equivalent cables. The cables included with
Pump It Up are custom and are flipped vertically instead of horizontally, which is non-standard. Since you cannot
buy these cables easily, brokeIO is instead designed to use common 34-pin 2.54mm IDC cables, which can readily be found
online. Unless you have a custom board, do **not** use the existing cables that connect the PIUIO and JAMMA!

Floppy cables will not work if a few of the wires in the middle are twisted (for floppy drives marked as A:). Cables
for floppy drives marked as B: do not have this twist and should be okay to use.

Using the wrong cable *shouldn't* break your cabinet or your brokeIO, but your inputs and outputs will be completely
wrong. You will know if you are using the wrong cable if the P1 and P2 down right lights are on when starting up.

## Configuration

There is a config mode built into the brokeIO which allows you to switch between different USB modes. To enter it,
hold the service button for two seconds. The bass neon and the 4th marquee light should illuminate. The other 3 marquee
lights as well as the up left, center, and up right arrows of each pad will blink according to the mode selected, based
on the binary representation of the mode's number. The JAMMA LED will also flicker based on the mode's number in case
you are not able to plug in the brokeIO to a complete setup.

| Number | Lights | Mode |
| --- | --- | --- |
| 0 | --- | PIUIO |
| 1 | --O | Gamepad (DInput) |
| 2 | -O- | LXIO |
| 3 | -OO | Keyboard |
| 4 | O-- | XInput (Xbox 360 Controller) |
| 5 | O-O | Nintendo Switch |
| 6 | OO- | Nintendo GameCube Adapter |
| 7 | OOO | Serial CDC |

To select a mode, choose with the down left and down right arrows of either pad, or use the test button. To exit the
config mode, hold the service button for two seconds, and the brokeIO will save your settings and reset.

To update the firmware, in config mode, hold P2 up left, up right and down right simultaneously. For ITG dedicabs, instead hold 2P up, down, and start simultaneously. Alternatively, while
holding the BOOT button, either plug in the brokeIO or press the RESET button if plugged in already. A storage device
called RPI-RP2 will show up on the host computer, and you can drag or copy the .UF2 firmware file into the drive, which
will automatically upload and flash the brokeIO.

If you accidentally enter the firmware uploading mode, resetting or unplugging/replugging the brokeIO will exit it.

## Troubleshooting

**Inputs are working, but lights aren't**
12V is required. Make sure that the 4-pin Molex connector is receiving power.
In addition, make sure that the IDC cables are firmly connected between the brokeIO and JAMMA PCBs.

**Inputs and outputs are behaving erratically and/or don't respond**
You may be using the wrong IDC cables, or you have a grounding issue affecting your machine or power supply.
If nothing is happening, make sure you are on the correct mode for your system.

**1P and 2P pad lights are swapped for ITG dedicabs**
Enter config mode, then press the 1P and 2P up panels simultaneously. The pad lights will turn on in order from 1P Left all the way to 2P Right (`1P Left → 1P Down → 1P Up → 1P Right → 2P Left → 2P Down → 2P Up → 2P Right`) to confirm the current orientation. This swaps the 1P and 2P pad lights for LXIO and gamepad modes (PIUIO mode should not have the lights swapped). Repeat this process to swap back.

## FAQ/Notes

**How do I reprogram the board?**
Hold the BOOT button when plugging in the brokeIO or pressing the RESET button. You do not need to plug in the Molex power cable to follow these steps, only the USB cable is needed. Alternatively, if you don't have physical access to the buttons and the brokeIO is plugged in, enter the config menu with the service button and press P2 UL+UR+DR (for ITG dedicabs press P2 UP+DOWN+START).
Then the device should show up as a flash drive called RPI-BOOT. Finally, you can drop the UF2 firmware file in the flash drive and it will upload the code automatically.

**Can I wire lights directly to this?**
Yes, but it is not recommended to power anything other than LEDs, since it may take up a lot of current.
The board is designed to accept ~500mA maximum for a single light output and ~1A for all lights at once. LEDs generally
take up way less than this, so you'd probably be fine, but definitely do the math in case. I cannot officially provide
support since this is not an intended use case.

**Can I power lights with something other than 12V?**
The 12V line should accept anything between 5V and 12V, if you would prefer to output a different voltage for lights and your system supports it.

**How do I adjust the keybinds in the firmware?**
This is not implemented yet but is a work in progress currently.

**Can I use my button board with brokeIO?**
ITG dedicab button boards are now supported.  PIU button board support is also not implemented yet but is a work in progress currently.

## Resources

### Pinouts

![PIUIO IDC connector pinouts](/docs/piuio.png)

![JAMMA edge and light connector pinouts](/docs/jamma.png)
