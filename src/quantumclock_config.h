#define LGFX_USE_V1
#include <pins_display.h>
#include <LovyanGFX.hpp>

#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <driver/i2c.h>

// Deine eigene Konfigurationsklasse
class LGFX : public lgfx::LGFX_Device
{
public:
  lgfx::Bus_RGB     _bus_instance;
  lgfx::Panel_RGB   _panel_instance;
  lgfx::Light_PWM   _light_instance;  

  LGFX(void)
  {
    { // panel settings - resolution
      auto cfg = _panel_instance.config();

      cfg.memory_width  = LCD_HRES;
      cfg.memory_height = LCD_VRES;
      cfg.panel_width  = LCD_HRES;
      cfg.panel_height = LCD_VRES;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      // cfg.pin_rst = LCD_RESET;

      _panel_instance.config(cfg);
    }

    { // panel settings details - use PSRAM
      auto cfg = _panel_instance.config_detail();

      cfg.use_psram = 1;
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
      cfg.freq_write  = 15000000;

      cfg.hsync_polarity = 1;
      cfg.hsync_front_porch = 10;
      cfg.hsync_pulse_width = 8;
      cfg.hsync_back_porch = 50;
      cfg.vsync_polarity = 1;
      cfg.vsync_front_porch = 10;
      cfg.vsync_pulse_width = 8;
      cfg.vsync_back_porch = 20;
      cfg.pclk_idle_high = 0;
      cfg.de_idle_high = 0;
      cfg.pclk_active_neg = 0;

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