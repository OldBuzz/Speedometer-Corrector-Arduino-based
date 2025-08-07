# Speedometer-Corrector-Arduino-based
Digital speedometer corrector, based on an Arduino Pro Mini.

This is an Arduino Pro Mini project for correcting a speedometer when you change tire size or differential gearing. It was initially designed for a Jeep Wrangler TJ (1997-2006). It may also work on other vehicles if they use a 5 volt squarewave signal from the speed sensor to the speedometer or to the PCM/ECU, within the range of 0 Hz to a little over 200 Hz. Some vehicles use a 12 volt speedometer signal, which would require several changes to this circuit to work. Some vehicles may use other signals, like a sinewave, or a CAN bus serial stream. This circuit will not work for those vehicles.

## Important Note

Installing this device involves splicing into your vehicle wiring, and altering the way a sensor reports to the PCM/ECU (engine computer). If you're uncomfortable with doing that, don't. An error could be very costly. It also requires you to burn a program into an Arduino Pro Mini. The Mini does not have a USB connection like the standard Arduino Uno, so you will need a USB-to-serial dongle, like an FTDI Breakout board. Or you can use a Nano for a small-footprint Arduino with a USB connector. And you can use a Uno instead of a Mini or a Nano. It will be bigger, but it's easier to burn. With a Uno, you can put this circuit on a "hat".

## How It Works

The Speedo Corrector is a small box which goes between the Vehicle Speed Sensor on your transmission or transfer case and your engine computer (PCM/ECU). It alters the signal to correct for a change in tire diameter or gearing.

First, you take a drive to compare the indicated speed on your speedometer with a GPS speed. You'll do a simple calculation to get a "correction factor", which you'll enter into the Speedo Corrector on a set of 8 switches.

The Speedo Corrector measures the period of the incoming squarewave from the speed sensor, and produces a longer or shorter output squarewave, depending on the correction factor you set.

## Original Designer

The hardware and firmware are based on Tom M'Guinness's 1986 Porsche 944 Turbo Speedometer Calibrator.  https://www.carpokes.com/viewtopic.php?t=558

This work is licensed under a Creative Commons Attribution-NonCommercial 4.0 International License (cc-by-nc), a copy of which can be found here:   https://creativecommons.org/licenses/by-nc/4.0/

## Hardware

I modified the hardware, removing the potentiometer and adding a DIP-switch, removing the voltage regulator since this will be driven from the regulated PCM sensor voltage, adding an open collector transistor to drive the pull-down output, and removing the 74C14 Schmitt Trigger since the Arduino has those built in to the digital ports. Instead of an Arduino Nano, I used an Arduino Pro Mini.

You can build this simple circuit on a sea-of-holes proto board, wiring it point-to-point. Or you can make a real PC board, but you'll have to lay it out yourself.

## Firmware

I modified the source code a little, mainly to read the correction factor from the DIP-switch. But it's still mostly Tom-code. You just have to burn it into the Arduino. That will require the Arduino IDE, and a serial-to-USB device (for the Mini). Or, you could switch to one of the other Arduinos with built-in USB, like the Nano or Uno.

## Where can I get one?

You'll have to acquire the parts and build it yourself.

## Related Projects

Speedometer-Corrector-PIC-based: A PIC18F1320-based speedometer corrector circuit with C-source firmware.

https://github.com/OldBuzz/Speedometer-Corrector-PIC-based

(Instead of making this Arduino-based one smaller, I designed a new one based on a PIC 18F1320. It's fairly similar hardware-wise, but the firmware is all new, because it uses a completely different way of timing the sensor pulses. The PIC chip is available in surface mount, so the device can be made much smaller.)
