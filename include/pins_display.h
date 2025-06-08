// pins_display.h

#ifndef PINS_DISPLAY_H
#define PINS_DISPLAY_H
#include <Arduino.h>

// --- Display Pinout (from 40-pin datasheet 0620A003-G1I0400.pdf - TOP to BOTTOM FPC Pin 1-40) ---
// Und ESP32-S3 zu Display FPC Pin Mapping (DIREKT AUS DER VOM BENUTZER BEREITGESTELLTEN TRACE image_144d54.jpg) ---
// WICHTIG: Diese Zuordnung wurde direkt aus den farbcodierten Leiterbahnen und Pin-Bezeichnungen
//          im Bild "image_144d54.jpg" abgeleitet.
//          Sie geht von einer 16-Bit parallelen RGB (RGB565) Schnittstelle aus, da R0-R2, G0-G1, B0-B2 auf Masse liegen.
//          FPC Pin 35 (NC) und FPC Pin 37 (SDD) sind in dieser spezifischen Schaltung ebenfalls auf Masse gezogen.
//          Schwarze Leiterbahnen zeigen Masse (GND) an.

// FPC Pinbelegungsreferenz (von OBEN nach UNTEN am linken Rand des Steckers in image_144d54.jpg):
// FPC Pin | Signal (aus PDF) | Leiterbahnfarbe (image_144d54.jpg) | ESP32-S3 GPIO Pin (aus image_144d54.jpg) | Abgeleitetes Signal (RGB565) / Anmerkungen
// --------|-------------------|------------------------------------|-------------------------------------------|-----------------------------------------------------
// 1       | LEDK              | Keine sichtbare Leiterbahn         | -                                         | LED Kathode (Hintergrundbeleuchtung)
// 2       | LEDA              | Keine sichtbare Leiterbahn         | -                                         | LED Anode (Hintergrundbeleuchtung)
// 3       | GND               | Schwarz                            | -                                         | Masse (Ground)
// 4       | VCC               | Blau                               | (ESP32 Seite VCC)                         | Display-Stromversorgung (verbunden mit 3.3V Schiene)
// 5       | R0                | Auf GND gezogen (Benutzerinfo)     | -                                         | Rote Daten 0 (auf GND für RGB565)
// 6       | R1                | Auf GND gezogen (Benutzerinfo)     | -                                         | Rote Daten 1 (auf GND für RGB565)
// 7       | R2                | Auf GND gezogen (Benutzerinfo)     | -                                         | Rote Daten 2 (auf GND für RGB565)
// 8       | R3                | Orange                             | GPIO0                                     | LCD_R0 (Rotes LSB für RGB565)
// 9       | R4                | Orange                             | GPIO1                                     | LCD_R1
// 10      | R5                | Orange                             | GPIO2                                     | LCD_R2
// 11      | R6                | Orange                             | GPIO3                                     | LCD_R3
// 12      | R7                | Orange                             | GPIO4                                     | LCD_R4 (Rotes MSB für RGB565)
// 13      | G0                | Auf GND gezogen (Benutzerinfo)     | -                                         | Grüne Daten 0 (auf GND für RGB565)
// 14      | G1                | Auf GND gezogen (Benutzerinfo)     | -                                         | Grüne Daten 1 (auf GND für RGB565)
// 15      | G2                | Grün                               | 6                                         | LCD_G0 (Grünes LSB für RGB565)
// 16      | G3                | Grün                               | 7                                         | LCD_G1
// 17      | G4                | Grün                               | 8                                         | LCD_G2
// 18      | G5                | Grün                               | 9                                         | LCD_G3
// 19      | G6                | Grün                               | 10                                        | LCD_G4
// 20      | G7                | Grün                               | 11                                        | LCD_G5 (Grünes MSB für RGB565)
// 21      | B0                | Auf GND gezogen (Benutzerinfo)     | -                                         | Blaue Daten 0 (auf GND für RGB565)
// 22      | B1                | Auf GND gezogen (Benutzerinfo)     | -                                         | Blaue Daten 1 (auf GND für RGB565)
// 23      | B2                | Auf GND gezogen (Benutzerinfo)     | -                                         | Blaue Daten 2 (auf GND für RGB565)
// 24      | B3                | Hellblau                           | 12                                        | LCD_B0 (Blaues LSB für RGB565)
// 25      | B4                | Hellblau                           | 13                                        | LCD_B1
// 26      | B5                | Hellblau                           | 14                                        | LCD_B2
// 27      | B6                | Hellblau                           | 15                                        | LCD_B3
// 28      | B7                | Hellblau                           | 16                                        | LCD_B4 (Blaues MSB für RGB565)
// 29      | GND               | Schwarz                            | -                                         | Masse (Ground)
// 30      | CLK               | Hellblau                           | 38                                        | LCD_PCLK (Pixel Takt)
// 31      | RESET             | Lila                               | 39                                        | LCD_RESET (Display Reset)
// 32      | Hsync             | Blau                               | 40                                        | LCD_HSYNC (Horizontale Synchronisation)
// 33      | Vsync             | Blau                               | 41                                        | LCD_VSYNC (Vertikale Synchronisation)
// 34      | DE                | Blau                               | 42                                        | LCD_DEN (Data Enable)
// 35      | NC                | Auf GND gezogen (Benutzerinfo)     | -                                         | Nicht verbunden (auf Masse gezogen)
// 36      | GND               | Schwarz                            | -                                         | Masse (Ground)
// 37      | SDD               | Auf GND gezogen (Benutzerinfo)     | -                                         | Serielle Dateneingabe (auf Masse gezogen) - Wahrscheinlich für SPI/I2C Konfiguration des Display-ICs, aber hier nicht verwendet.
// 38      | CS                | Grün                               | 46                                        | LCD_CS (Chip Select für den Display-Controller)
// 39      | SCL               | Rot                                | 47                                        | LCD_SCL (Serieller Takt für den Display-Controller)
// 40      | SDA               | Rot                                | 48                                        | LCD_SDA (Serielle Daten für den Display-Controller)


// --- Zugeordnete Defines für Arduino/ESP-IDF ---
// Verwendung der tatsächlichen GPIO-Nummern des ESP32-S3, wie im Arduino-Core definiert.

// Rote Datenpins (5 Bits für RGB565, abgeleitet von R3-R7 des RGB888)
#define LCD_R0    GPIO_NUM_0 // FPC Pin 8  (ESP32 - GPIO0 - ESP pin 5) - Rotes LSB
#define LCD_R1    GPIO_NUM_1 // FPC Pin 9  (ESP32 - GPIO1 - ESP pin 6) - Rotes 2
#define LCD_R2    GPIO_NUM_2 // FPC Pin 10 (ESP32 - GPIO2) - ESP pin 7 - Rotes 3
#define LCD_R3    GPIO_NUM_3 // FPC Pin 11 (ESP32 - GPIO3) - ESP pin 8 - Rotes 4
#define LCD_R4    GPIO_NUM_4 // FPC Pin 12 (ESP32 - GPIO4) - ESP pin 9 - Rotes MSB

// Grüne Datenpins (6 Bits für RGB565, abgeleitet von G2-G7 des RGB888)
#define LCD_G0    GPIO_NUM_6 // FPC Pin 15 (ESP32 - GPIO6 - pin 11) - Grünes LSB
#define LCD_G1    GPIO_NUM_7 // FPC Pin 16 (ESP32 - GPIO7 - pin 12) - Grünes 2
#define LCD_G2    GPIO_NUM_8 // FPC Pin 17 (ESP32 - GPIO8 - pin 13) - Grünes 3
#define LCD_G3    GPIO_NUM_9 // FPC Pin 18 (ESP32 - GPIO9 - pin 14) - Grünes 4
#define LCD_G4    GPIO_NUM_10 // FPC Pin 19 (ESP32 - GPIO10 - pin 15) - Grünes 5
#define LCD_G5    GPIO_NUM_11 // FPC Pin 20 (ESP32 - GPIO11 - pin 16) - Grünes MSB

// Blaue Datenpins (5 Bits für RGB565, abgeleitet von B3-B7 des RGB888)
#define LCD_B0    GPIO_NUM_12 // FPC Pin 24 (ESP32 - GPIO12 - pin 17) - Blaues LSB
#define LCD_B1    GPIO_NUM_13 // FPC Pin 25 (ESP32 - GPIO13 - pin 18) - Blaues 2
#define LCD_B2    GPIO_NUM_14 // FPC Pin 26 (ESP32 - GPIO14 - pin 19) - Blaues 3
#define LCD_B3    GPIO_NUM_15 // FPC Pin 27 (ESP32 - GPIO15 - pin 21) - Blaues 4
#define LCD_B4    GPIO_NUM_16 // FPC Pin 28 (ESP32 - GPIO16 - pin 22) - Blaues MSB

// Steuerpins (Gängige Parallel-RGB-Signale)
#define LCD_PCLK  GPIO_NUM_17 // FPC Pin 30 (ESP32 - GPIO17 - pin 23) - Pixel Takt
#define LCD_RESET GPIO_NUM_46 // FPC Pin 31 (ESP32 - GPIO46 - pin 52) - Display Reset
#define LCD_HSYNC GPIO_NUM_21 // FPC Pin 32 (ESP32 - GPIO21 - pin 27) - Horizontale Synchronisation
#define LCD_VSYNC SPI_CLK_P// FPC Pin 33 (ESP32 - SPICLK_P - pin 36) - Vertikale Synchronisation
#define LCD_DEN   GPIO_NUM_18 // FPC Pin 34 (ESP32 - GPIO18 - pin 24) - Data Enable

// Serielle Steuerschnittstelle (für Display-Controller-Konfiguration, z.B. GC9503CV)
#define LCD_CS    MTD0 // FPC Pin 38 (ESP32 - MTd0 - pin 45) - Chip Select
#define LCD_SCL   MTCK // FPC Pin 39 (ESP32 - MTCK - pin 44) - Serieller Takt
#define LCD_SDA   GPIO_NUM_38 // FPC Pin 40 (ESP32 - GPIO38 - pin 43) - Serielle Daten

// Hintergrundbeleuchtungssteuerung (LEDK/LEDA FPC Pins 1/2)
// Diese Pins sind mit dem dedizierten Hintergrundbeleuchtungs-Treiber-IC auf der Hauptplatine verbunden (U7/L6-Schaltung).
// Die spezifischen GPIOs des ESP32-S3 für die Steuerung der Hintergrundbeleuchtung (z.B. PWM für Helligkeit oder Enable)
// sind in der bereitgestellten FPC-Leiterbahnverfolgung nicht direkt sichtbar. Sie könnten über andere Leiterbahnen
// auf der Hauptplatine oder indirekt gesteuert werden.


#endif // PINS_DISPLAY_H