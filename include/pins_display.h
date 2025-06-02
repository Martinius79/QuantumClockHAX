// pins_display.h

#ifndef PINS_DISPLAY_H
#define PINS_DISPLAY_H

// --- Display Pinout (from 40-pin datasheet 0620A003-G1I0400) ---
// Pin | Signal
// ----|--------------------
// 1   | GND
// 2   | LEDK
// 3   | LEDA
// 4   | VCC
// 5   | D0
// 6   | D1
// 7   | D2
// 8   | D3
// 9   | D4
// 10  | D5
// 11  | D6
// 12  | D7
// 13  | D8
// 14  | D9
// 15  | D10
// 16  | D11
// 17  | D12
// 18  | D13
// 19  | D14
// 20  | D15
// 21  | GND
// 22  | DVSYNC
// 23  | DHSYNC
// 24  | DEN
// 25  | DCLK
// 26  | GND
// 27  | RESET
// 28  | CS
// 29  | DC
// 30  | SCL
// 31  | SDA
// 32  | /RD
// 33  | /WR
// 34  | NC
// 35  | NC
// 36  | GND
// 37  | VGH
// 38  | VGL
// 39  | VCOM
// 40  | GND

// --- ESP32-S3 to Display FPC Pin Mapping (Derived from Visual Inspection) ---
// IMPORTANT: This mapping is derived from visual inspection of PCB traces.
//            A physical multimeter continuity check or the official schematic
//            is highly recommended for 100% accuracy.

// Data Pins (16-bit parallel interface)
#define LCD_D0   21 // FPC Pin 5  (ESP32-S3 GPIO21)
#define LCD_D1   20 // FPC Pin 6  (ESP32-S3 GPIO20)
#define LCD_D2   19 // FPC Pin 7  (ESP32-S3 GPIO19)
#define LCD_D3   18 // FPC Pin 8  (ESP32-S3 GPIO18)
#define LCD_D4   17 // FPC Pin 9  (ESP32-S3 GPIO17)
#define LCD_D5   16 // FPC Pin 10 (ESP32-S3 GPIO16)
#define LCD_D6   15 // FPC Pin 11 (ESP32-S3 GPIO15)
#define LCD_D7   14 // FPC Pin 12 (ESP32-S3 GPIO14)
#define LCD_D8   13 // FPC Pin 13 (ESP32-S3 GPIO13)
#define LCD_D9   12 // FPC Pin 14 (ESP32-S3 GPIO12)
#define LCD_D10  11 // FPC Pin 15 (ESP32-S3 GPIO11)
#define LCD_D11  10 // FPC Pin 16 (ESP32-S3 GPIO10)
#define LCD_D12  9  // FPC Pin 17 (ESP32-S3 GPIO9)
#define LCD_D13  8  // FPC Pin 18 (ESP32-S3 GPIO8)
#define LCD_D14  7  // FPC Pin 19 (ESP32-S3 GPIO7)
#define LCD_D15  6  // FPC Pin 20 (ESP32-S3 GPIO6)

// Control Pins
#define LCD_VSYNC 5  // FPC Pin 22 (ESP32-S3 GPIO5)
#define LCD_HSYNC 4  // FPC Pin 23 (ESP32-S3 GPIO4)
#define LCD_DEN   3  // FPC Pin 24 (ESP32-S3 GPIO3)
#define LCD_PCLK  2  // FPC Pin 25 (ESP32-S3 GPIO2) (Pixel Clock)
#define LCD_RESET 1  // FPC Pin 27 (ESP32-S3 GPIO1)
#define LCD_CS    0  // FPC Pin 28 (ESP32-S3 GPIO0) (Chip Select for display controller)

// --- Uncertainties / Pins that require verification ---
// The following pins could not be clearly traced due to obstructions or complex routing.
// A multimeter continuity check is crucial for these:

// #define LCD_DC    ?  // FPC Pin 29 (Data/Command for display controller)
// #define LCD_SCL   ?  // FPC Pin 30 (Serial Clock for display controller, e.g., I2C/SPI)
// #define LCD_SDA   ?  // FPC Pin 31 (Serial Data for display controller, e.g., I2C/SPI)
// #define LCD_RD    ?  // FPC Pin 32 (Read Strobe - often not used in write-only setups)
// #define LCD_WR    ?  // FPC Pin 33 (Write Strobe - often not used in write-only setups)

// Backlight Control (LEDA/LEDK - FPC Pins 2/3)
// These typically connect to a backlight driver circuit. The control signal for this driver
// (e.g., for PWM dimming) would then connect to an ESP32-S3 GPIO. This connection is not visible.
// #define LCD_BL    ?  // GPIO for backlight control (e.g., PWM for brightness)

#endif // PINS_DISPLAY_H