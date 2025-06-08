#define LGFX_USE_V1
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

      _panel_instance.config(cfg);
    }

    { // panel settings details - use PSRAM
      auto cfg = _panel_instance.config_detail();

      cfg.use_psram = 1;

      _panel_instance.config_detail(cfg);
    }

    { // bus configuration - set pinout (GC9503CV)
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;
      cfg.pin_d0  = GPIO_NUM_0;  // B0
      cfg.pin_d1  = GPIO_NUM_1;  // B1
      cfg.pin_d2  = GPIO_NUM_2; // B2
      cfg.pin_d3  = GPIO_NUM_3;  // B3
      cfg.pin_d4  = GPIO_NUM_4;  // B4
      cfg.pin_d5  = GPIO_NUM_6;  // G0
      cfg.pin_d6  = GPIO_NUM_7;  // G1
      cfg.pin_d7  = GPIO_NUM_8;  // G2
      cfg.pin_d8  = GPIO_NUM_9; // G3
      cfg.pin_d9  = GPIO_NUM_10; // G4
      cfg.pin_d10 = GPIO_NUM_11;  // G5
      cfg.pin_d11 = GPIO_NUM_12; // R0
      cfg.pin_d12 = GPIO_NUM_13; // R1
      cfg.pin_d13 = GPIO_NUM_14; // R2
      cfg.pin_d14 = GPIO_NUM_15; // R3
      cfg.pin_d15 = GPIO_NUM_16; // R4

      cfg.pin_henable = GPIO_NUM_18; // DE (Data Enable)
      cfg.pin_vsync   = GPIO_NUM_40; // VSYNC
      cfg.pin_hsync   = GPIO_NUM_21; // HSYNC
      cfg.pin_pclk    = GPIO_NUM_17; // PCLK (Pixel Clock)
      cfg.freq_write  = 12000000;

      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 2;
      cfg.hsync_back_porch  = 43;
      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 2;
      cfg.vsync_back_porch  = 12;
      cfg.pclk_idle_high    = 1;
      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = GPIO_NUM_45;
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