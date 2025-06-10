#define LGFX_USE_V1
#include <pins_display.h>
#include <LovyanGFX.hpp>

#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

// for IPSTUBE Quantum Clock
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

#define LCD_HRES  960 // Horizontale Auflösung des Displays (Breite)
#define LCD_VRES  360 // Vertikale Auflösung des Displays (Höhe)

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
#define LCD_VSYNC GPIO_NUM_47 // FPC Pin 33 (ESP32 - SPICLK_P - pin 36) - Vertikale Synchronisation
#define LCD_DEN   GPIO_NUM_18 // FPC Pin 34 (ESP32 - GPIO18 - pin 24) - Data Enable

// Serielle Steuerschnittstelle (für Display-Controller-Konfiguration, z.B. GC9503CV)
#define LCD_CS    GPIO_NUM_40 // FPC Pin 38 (ESP32 - MTd0 - pin 45) - Chip Select
#define LCD_SCL   GPIO_NUM_39 // FPC Pin 39 (ESP32 - MTCK - pin 44) - Serieller Takt
#define LCD_SDA   GPIO_NUM_38 // FPC Pin 40 (ESP32 - GPIO38 - pin 43) - MOSI - Serielle Daten
#define LCD_SDO   -1 // FPC Pin 37 - Ground (ESP32 - NC) - MISO - Serielle Dateneingabe (NC in dieser Schaltung, auf GND gezogen)

// --- Hintergrundbeleuchtung (LEDK/LEDA) ---
#define LCD_BLK  GPIO_NUM_45 // FPC Pin 1 (LEDK) - Hintergrundbeleuchtung Kathode


// Hintergrundbeleuchtungssteuerung (LEDK/LEDA FPC Pins 1/2)
// Diese Pins sind mit dem dedizierten Hintergrundbeleuchtungs-Treiber-IC auf der Hauptplatine verbunden (U7/L6-Schaltung).
// Die spezifischen GPIOs des ESP32-S3 für die Steuerung der Hintergrundbeleuchtung (z.B. PWM für Helligkeit oder Enable)
// sind in der bereitgestellten FPC-Leiterbahnverfolgung nicht direkt sichtbar. Sie könnten über andere Leiterbahnen
// auf der Hauptplatine oder indirekt gesteuert werden.
// Deine eigene Konfigurationsklasse

class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_GC9503   _panel_instance;
  lgfx::Light_PWM   _light_instance;

  LGFX(void)
  {
    { // panel settings - resolution
      auto cfg = _panel_instance.config();

      cfg.memory_width  = LCD_VRES;
      cfg.memory_height = LCD_HRES;
      cfg.panel_width  = LCD_VRES;
      cfg.panel_height = LCD_HRES;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      cfg.pin_rst = LCD_RESET;
      //cfg.pin_cs = LCD_CS;      

      _panel_instance.config(cfg);
    }

    { // panel settings details - use PSRAM
      auto cfg = _panel_instance.config_detail();

      //cfg.use_psram = 1;
      cfg.pin_cs = LCD_CS;
      cfg.pin_sclk = LCD_SCL;
      cfg.pin_mosi = LCD_SDA;

      _panel_instance.config_detail(cfg);
    }

    { // bus configuration - set pinout (GC9503CV)
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      cfg.pin_d0  = LCD_B0;  // B0
      cfg.pin_d1  = LCD_B1;  // B1
      cfg.pin_d2  = LCD_B2; // B2
      cfg.pin_d3  = LCD_B3;  // B3
      cfg.pin_d4  = LCD_B4;  // B4
      cfg.pin_d5  = LCD_G0;  // G0
      cfg.pin_d6  = LCD_G1;  // G1
      cfg.pin_d7  = LCD_G2;  // G2
      cfg.pin_d8  = LCD_G3; // G3
      cfg.pin_d9  = LCD_G4; // G4
      cfg.pin_d10 = LCD_G5;  // G5
      cfg.pin_d11 = LCD_R0; // R0
      cfg.pin_d12 = LCD_R1; // R1
      cfg.pin_d13 = LCD_R2; // R2
      cfg.pin_d14 = LCD_R3; // R3
      cfg.pin_d15 = LCD_R4; // R4

      cfg.pin_henable = LCD_DEN; // DE (Data Enable)
      cfg.pin_vsync   = LCD_VSYNC; // VSYNC
      cfg.pin_hsync   = LCD_HSYNC; // HSYNC
      cfg.pin_pclk    = LCD_PCLK; // PCLK (Pixel Clock)      
      // cfg.freq_write  = 15000000; //default 15MHz, but can be adjusted based on your display's requirements -> not enough for 960x360@60Hz
      // cfg.freq_write  = 25628640; // ~26MHz for 960x360@60Hz
      cfg.freq_write  = 32000000; // ~32MHz for 960x360@60Hz

      // from ???
      // cfg.hsync_polarity = 1;
      // cfg.hsync_front_porch = 10;
      // cfg.hsync_pulse_width = 8;
      // cfg.hsync_back_porch = 50;
      // cfg.vsync_polarity = 1;
      // cfg.vsync_front_porch = 10;
      // cfg.vsync_pulse_width = 8;
      // cfg.vsync_back_porch = 20;
      // cfg.pclk_idle_high = 0;
      // cfg.de_idle_high = 0;
      // cfg.pclk_active_neg = 0;

      //from HA_Deck
      cfg.hsync_polarity = 1;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 10;
      cfg.hsync_back_porch = 50;
      cfg.vsync_polarity = 1;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 10;
      cfg.vsync_back_porch = 20;
      cfg.pclk_idle_high = 0;
      cfg.de_idle_high = 0;
      cfg.pclk_active_neg = 0;

      //from LovyanGFX-GUITION-ESP32-4848S040
      // cfg.hsync_polarity    = 0;
      // cfg.hsync_front_porch = 10;
      // cfg.hsync_pulse_width = 8;
      // cfg.hsync_back_porch  = 50;
      // cfg.vsync_polarity    = 0;
      // cfg.vsync_front_porch = 10;
      // cfg.vsync_pulse_width = 8;
      // cfg.vsync_back_porch  = 20;
      // cfg.pclk_idle_high    = 0;
      // cfg.de_idle_high      = 1;

      // cfg.pclk_active_neg = 0;

      // from Gemini
      // cfg.freq_write        = 25628640; // Pixeltaktfrequenz (ca. 25.6 MHz für 60Hz)
      // cfg.hsync_polarity    = 0;        // HSYNC aktiv LOW
      // cfg.hsync_front_porch = 10;       // Horizontale Front Porch
      // cfg.hsync_pulse_width = 8;        // HSYNC Impulsbreite
      // cfg.hsync_back_porch  = 50;       // Horizontale Back Porch
      // cfg.vsync_polarity    = 0;        // VSYNC aktiv LOW
      // cfg.vsync_front_porch = 10;       // Vertikale Front Porch
      // cfg.vsync_pulse_width = 8;        // VSYNC Impulsbreite
      // cfg.vsync_back_porch  = 20;       // Vertikale Back Porch
      // cfg.pclk_idle_high    = 0;        // PCLK ist LOW im Idle-Zustand (aktive Flanke)
      // cfg.de_idle_high      = 0;        // DE ist LOW im Idle-Zustand
      // cfg.pclk_active_neg   = 0;        // PCLK ist aktiv auf positiver Flanke (0=positive, 1=negative)

      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = LCD_BLK;
      _light_instance.config(cfg);
    }
    _panel_instance.light(&_light_instance);

    // { // Panel-Konfiguration (GC9503CV)
    //   auto cfg = _panel_instance.config();

    //   cfg.memory_width     = 960; // Breite des Displays (z.B. 800)
    //   cfg.memory_height    = 360; // Höhe des Displays (z.B. 480)
    //   cfg.panel_width      = 960; // Tatsächliche Breite (kann von memory_width abweichen, wenn Controller mehr Speicher hat)
    //   cfg.panel_height     = 360; // Tatsächliche Höhe
    //   cfg.offset_x         = 0;
    //   cfg.offset_y         = 0;
    //   cfg.offset_rotation  = 0;
    //   cfg.dummy_read_pixel = 8;
    //   cfg.dummy_read_bits  = 1;
    //   cfg.readable         = false; // RGB-Displays sind oft nicht lesbar
    //   cfg.invert           = false; // Invertierte Farben?
    //   cfg.rgb_order        = false; // RGB-Reihenfolge (false für RGB, true für BGR)
    //   cfg.dlen_read_pixel  = 16;
    //   cfg.bus_shared       = false; // Ist der Bus mit anderen Geräten geteilt?

    //   // Timing-Parameter für HSYNC und VSYNC (sehr wichtig!)
    //   // Diese Werte findest du im Datenblatt des GC9503CV oder des Panels.
    //   // Sie werden oft als HBP, HFP, VBP, VFP (Horizontal/Vertical Back/Front Porch)
    //   // oder ähnliche Bezeichnungen angegeben.
    //   cfg.sync_h_polarity  = false; // HSYNC Polarität
    //   cfg.sync_v_polarity  = false; // VSYNC Polarität
    //   cfg.sync_h_front     = 40;    // Horizontal Front Porch
    //   cfg.sync_h_back      = 210;   // Horizontal Back Porch
    //   cfg.sync_h_width     = 10;    // HSYNC Pulsbreite
    //   cfg.sync_v_front     = 20;    // Vertical Front Porch
    //   cfg.sync_v_back      = 22;    // Vertical Back Porch
    //   cfg.sync_v_width     = 10;    // VSYNC Pulsbreite

    //   _panel_instance.config(cfg);
    // }

    setPanel(&_panel_instance);
  }
};