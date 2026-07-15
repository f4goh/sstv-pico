# RP2350 Touch LCD 2 SSTV Transmitter

This project is a natural continuation of the **WSPR-Pico** project:

https://github.com/f4goh/wspr-pico/

While WSPR-Pico demonstrated that the Raspberry Pi RP2040 could be used as a compact and efficient HF beacon, the **Waveshare RP2350 Touch LCD 2** provides an even more capable platform for building a standalone **HF SSTV transmitter**.

The board combines a powerful **Raspberry Pi RP2350** microcontroller with a bright **2-inch capacitive touch display**, making it ideal for generating, displaying, and transmitting SSTV images without additional hardware. The integrated LCD offers a convenient user interface for image selection, transmission control, and system monitoring.

## Main Features

- Raspberry Pi **RP2350** microcontroller
- Dual-core ARM Cortex-M33 and dual-core Hazard3 RISC-V architecture
- Up to **150 MHz** CPU clock
- **520 KB SRAM**
- **16 MB Flash**
- USB Type-C connector
- 2-inch IPS capacitive touch LCD
- 240 × 320 resolution (262K colors)
- Camera connector compatible with OV2640 and OV5640
- On-board Li-Ion battery charging connector
- USB 1.1 Host and Device support
- 22 multifunction GPIOs
- 2 × SPI, 2 × I²C, 2 × UART
- 14 PWM channels
- 12 Programmable I/O (PIO) state machines
- Temperature sensor
- Hardware floating-point support
- Low-power sleep and dormant modes

## Display

- Controller: **ST7789T3**
- Interface: SPI
- Resolution: 240 × 320 pixels
- IPS panel
- Capacitive touch controller: **CST816D** (I²C)

## IMU

The board includes a **QMI8658** inertial measurement unit featuring:

- 16-bit accelerometer
  - ±2, ±4, ±8, ±16 g ranges
- 16-bit gyroscope
  - ±16 to ±2048°/s ranges

## Board Overview

![RP2350 GPIO](images/RP2350-gpio.jpg)

![RP2350 Introduction](images/RP2350-intro.jpg)

![RP2350 Size](images/RP2350-size.jpg)

