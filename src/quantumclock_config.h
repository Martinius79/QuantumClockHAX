#ifndef QUANTUMCLOCK_CONFIG_H
#define QUANTUMCLOCK_CONFIG_H // for IPSTUBE H407 - Quantum Clock - Display

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

#include <Arduino.h> // for GPIO_NUM_x definitions

// --- Display Pinout (from 40-pin datasheet 0620A003-G1I0400.pdf - TOP to BOTTOM FPC Pin 1-40) ---

// FPC Pin | Signal (from PDF)  |  Connected to                             | Derived Signal (RGB565) / Notes
// --------|--------------------|--------------------------------------------|-----------------------------------------------------
// 1       | LEDK               | GPIO_NUM_45                                | LED cathode (backlight)
// 2       | LEDA               | GND                                        | LED anode (backlight)
// 3       | GND                | GND                                        | Ground
// 4       | VCC                | VCC                                        | Display power supply (connected to 3.3V rail)
// 5       | R0                 | GND                                        | Red data 0 (tied to GND for RGB565)
// 6       | R1                 | GND                                        | Red data 1 (tied to GND for RGB565)
// 7       | R2                 | GND                                        | Red data 2 (tied to GND for RGB565)
// 8       | R3                 | GPIO_NUM_0                                 | LCD_R0 (Red LSB for RGB565)
// 9       | R4                 | GPIO_NUM_1                                 | LCD_R1
// 10      | R5                 | GPIO_NUM_2                                 | LCD_R2
// 11      | R6                 | GPIO_NUM_3                                 | LCD_R3
// 12      | R7                 | GPIO_NUM_4                                 | LCD_R4 (Red MSB for RGB565)
// 13      | G0                 | GND                                        | Green data 0 (tied to GND for RGB565)
// 14      | G1                 | GND                                        | Green data 1 (tied to GND for RGB565)
// 15      | G2                 | GPIO_NUM_6                                 | LCD_G0 (Green LSB for RGB565)
// 16      | G3                 | GPIO_NUM_7                                 | LCD_G1
// 17      | G4                 | GPIO_NUM_8                                 | LCD_G2
// 18      | G5                 | GPIO_NUM_9                                 | LCD_G3
// 19      | G6                 | GPIO_NUM_10                                | LCD_G4
// 20      | G7                 | GPIO_NUM_11                                | LCD_G5 (Green MSB for RGB565)
// 21      | B0                 | GND                                        | Blue data 0 (tied to GND for RGB565)
// 22      | B1                 | GND                                        | Blue data 1 (tied to GND for RGB565)
// 23      | B2                 | GND                                        | Blue data 2 (tied to GND for RGB565)
// 24      | B3                 | GPIO_NUM_12                                | LCD_B0 (Blue LSB for RGB565)
// 25      | B4                 | GPIO_NUM_13                                | LCD_B1
// 26      | B5                 | GPIO_NUM_14                                | LCD_B2
// 27      | B6                 | GPIO_NUM_15                                | LCD_B3
// 28      | B7                 | GPIO_NUM_16                                | LCD_B4 (Blue MSB for RGB565)
// 29      | GND                | GND                                        | Ground
// 30      | CLK                | GPIO_NUM_17                                | LCD_PCLK (Pixel clock)
// 31      | RESET              | GPIO_NUM_46                                | LCD_RESET (Display reset)
// 32      | Hsync              | GPIO_NUM_21                                | LCD_HSYNC (Horizontal sync)
// 33      | Vsync              | GPIO_NUM_47                                | LCD_VSYNC (Vertical sync)
// 34      | DE                 | GPIO_NUM_18                                | LCD_DEN (Data Enable) - used to indicate valid data on the data bus (active high) - used for RGB565 mode -> Always high in this case after initialization
// 35      | NC                 | GND                                        | Not connected (tied to ground)
// 36      | GND                | GND                                        | Ground
// 37      | SDD                | GND                                        | Serial data input (tied to ground) - Probably for SPI backchannel (MISO) or I2C config of the display IC, but not used here.
// 38      | CS                 | GPIO_NUM_40                                | LCD_CS - for Three-Wire SDI here (Chip Select for the display controller - MTD0)
// 39      | SCL                | GPIO_NUM_39                                | LCD_SCL- for Three-Wire SDI here (Serial clock for the display controller - MTCK)
// 40      | SDA                | GPIO_NUM_38                                | LCD_SDA - for Three-Wire SDI here (Serial data for the display controller - MOSI)

// --- Assigned defines for Arduino/ESP-IDF ---
// Use the actual GPIO numbers of the ESP32-S3, as defined in the Arduino core.

#define LCD_HRES  960 // Horizontal resolution of the GC9503CV driver (width)
#define LCD_VRES  480 // Vertical resolution of the GC9503CV driver (height)

#define LCD_HRES_PHYS  960 // Physical horizontal resolution of the display (width)
#define LCD_VRES_PHYS  420 // Physical vertical resolution of the display (height)

// Red data pins (5 bits for RGB565, derived from R3-R7 of RGB888)
#define LCD_R0    GPIO_NUM_0 // FPC Pin 8  (ESP32 - GPIO0 - ESP pin 5) - Red LSB
#define LCD_R1    GPIO_NUM_1 // FPC Pin 9  (ESP32 - GPIO1 - ESP pin 6) - Red 2
#define LCD_R2    GPIO_NUM_2 // FPC Pin 10 (ESP32 - GPIO2) - ESP pin 7 - Red 3
#define LCD_R3    GPIO_NUM_3 // FPC Pin 11 (ESP32 - GPIO3) - ESP pin 8 - Red 4
#define LCD_R4    GPIO_NUM_4 // FPC Pin 12 (ESP32 - GPIO4) - ESP pin 9 - Red MSB

// Green data pins (6 bits for RGB565, derived from G2-G7 of RGB888)
#define LCD_G0    GPIO_NUM_6 // FPC Pin 15 (ESP32 - GPIO6 - pin 11) - Green LSB
#define LCD_G1    GPIO_NUM_7 // FPC Pin 16 (ESP32 - GPIO7 - pin 12) - Green 2
#define LCD_G2    GPIO_NUM_8 // FPC Pin 17 (ESP32 - GPIO8 - pin 13) - Green 3
#define LCD_G3    GPIO_NUM_9 // FPC Pin 18 (ESP32 - GPIO9 - pin 14) - Green 4
#define LCD_G4    GPIO_NUM_10 // FPC Pin 19 (ESP32 - GPIO10 - pin 15) - Green 5
#define LCD_G5    GPIO_NUM_11 // FPC Pin 20 (ESP32 - GPIO11 - pin 16) - Green MSB

// Blue data pins (5 bits for RGB565, derived from B3-B7 of RGB888)
#define LCD_B0    GPIO_NUM_12 // FPC Pin 24 (ESP32 - GPIO12 - pin 17) - Blue LSB
#define LCD_B1    GPIO_NUM_13 // FPC Pin 25 (ESP32 - GPIO13 - pin 18) - Blue 2
#define LCD_B2    GPIO_NUM_14 // FPC Pin 26 (ESP32 - GPIO14 - pin 19) - Blue 3
#define LCD_B3    GPIO_NUM_15 // FPC Pin 27 (ESP32 - GPIO15 - pin 21) - Blue 4
#define LCD_B4    GPIO_NUM_16 // FPC Pin 28 (ESP32 - GPIO16 - pin 22) - Blue MSB

// Control pins (common parallel RGB signals)
#define LCD_PCLK  GPIO_NUM_17 // FPC Pin 30 (ESP32 - GPIO17 - pin 23) - Pixel clock
#define LCD_RESET GPIO_NUM_46 // FPC Pin 31 (ESP32 - GPIO46 - pin 52) - Display reset
#define LCD_HSYNC GPIO_NUM_21 // FPC Pin 32 (ESP32 - GPIO21 - pin 27) - Horizontal sync
#define LCD_VSYNC GPIO_NUM_47 // FPC Pin 33 (ESP32 - SPICLK_P - pin 36) - Vertical sync
#define LCD_DEN   GPIO_NUM_18 // FPC Pin 34 (ESP32 - GPIO18 - pin 24) - Data Enable

// Serial control interface (for display controller configuration, GC9503CV)
// Using Three-Wire SDI (Serial Data Interface) with Chip Select, Serial Clock, and Serial Data, no DE or MISO (Master In Slave Out) used in this configuration.
#define LCD_CS    GPIO_NUM_40 // FPC Pin 38 (ESP32 - MTD0 - pin 45) - Chip Select
#define LCD_SCL   GPIO_NUM_39 // FPC Pin 39 (ESP32 - MTCK - pin 44) - Serial clock
#define LCD_SDA   GPIO_NUM_38 // FPC Pin 40 (ESP32 - GPIO38 - pin 43) - MOSI - Serial data
#define LCD_SDO   -1 // FPC Pin 37 - Ground (ESP32 - NC) - MISO - Serial data input (in this circuit, tied to GND)

// --- Backlight (LEDK/LEDA) ---
// Control of the backlight (PWM for brightness possible or just enable/disable)
#define LCD_BLK  GPIO_NUM_45 // FPC Pin 1 (LEDK) - Backlight cathode

class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_GC9503_Quantum_Clock   _panel_instance;
  lgfx::Light_PWM   _light_instance;

  LGFX(void)
  {
    { // panel settings - resolution
      auto cfg = _panel_instance.config();

      // Set the physical resolution of the panel
      // I have NO IDEA why VRES has to be 420! // But it works with 420, otherwise the display is showing stripes!
      cfg.panel_width  = LCD_VRES_PHYS;
      cfg.panel_height = LCD_HRES_PHYS;
      
      // Set the memory resolution of the panel (this is the resolution of the framebuffer) "full" size of the display driver GC9503CV is 960x480, so we use that
      cfg.memory_width  = LCD_VRES;
      cfg.memory_height = LCD_HRES;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      cfg.pin_rst = LCD_RESET;
      cfg.pin_cs = LCD_CS;

      _panel_instance.config(cfg);
    }

    { // panel settings details - use PSRAM
      auto cfg = _panel_instance.config_detail();

      cfg.use_psram = 2; // 0=SRAM only (do not use PSRAM) / 1=both(half PSRAM and half SRAM) / 2=PSRAM only (do not use SRAM)

      cfg.pin_cs   = LCD_CS;
      cfg.pin_sclk = LCD_SCL;
      cfg.pin_mosi = LCD_SDA;

      _panel_instance.config_detail(cfg);
    }

    { // bus configuration - set pinout (GC9503CV)
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      // RGB 16-bit parallel interface
      cfg.pin_d0  = LCD_B0; // B0
      cfg.pin_d1  = LCD_B1; // B1
      cfg.pin_d2  = LCD_B2; // B2
      cfg.pin_d3  = LCD_B3; // B3
      cfg.pin_d4  = LCD_B4; // B4
      cfg.pin_d5  = LCD_G0; // G0
      cfg.pin_d6  = LCD_G1; // G1
      cfg.pin_d7  = LCD_G2; // G2
      cfg.pin_d8  = LCD_G3; // G3
      cfg.pin_d9  = LCD_G4; // G4
      cfg.pin_d10 = LCD_G5; // G5
      cfg.pin_d11 = LCD_R0; // R0
      cfg.pin_d12 = LCD_R1; // R1
      cfg.pin_d13 = LCD_R2; // R2
      cfg.pin_d14 = LCD_R3; // R3
      cfg.pin_d15 = LCD_R4; // R4

      cfg.pin_henable = LCD_DEN; // DE (Data Enable)
      cfg.pin_vsync   = LCD_VSYNC; // VSYNC
      cfg.pin_hsync   = LCD_HSYNC; // HSYNC
      cfg.pin_pclk    = LCD_PCLK; // PCLK (Pixel Clock)

      //polarity
      cfg.hsync_polarity = true; 
      cfg.vsync_polarity = true;

      //edges
      cfg.pclk_idle_high  = false;
      cfg.de_idle_high    = false;
      cfg.pclk_active_neg = false;

      // pixel clock frequency (in Hz)
      cfg.freq_write = 13000000; // ~13MHz
      // cfg.freq_write  = 15000000; // default 15MHz, but can be adjusted based on your display's requirements -> not enough for 960x360@30Hz
      // cfg.freq_write = 16651080; // ~16.65MHz      
      // cfg.freq_write = 33302160; // ~33.3MHz

      // working, but when drawing a lot of pixels, the display is flickering and glitching
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 10;
      cfg.hsync_back_porch = 50;

      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 10;
      cfg.vsync_back_porch = 20;

      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = LCD_BLK;
      _light_instance.config(cfg);
    }
    _panel_instance.light(&_light_instance);

    setPanel(&_panel_instance);
  }
};

// --- End of Display Pinout and Configuration ---
#endif // QUANTUMCLOCK_CONFIG_H
// EOF