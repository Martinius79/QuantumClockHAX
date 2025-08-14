/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#if defined(ESP_PLATFORM)
#include <sdkconfig.h>
#if defined(CONFIG_IDF_TARGET_ESP32S3)

#include "Panel_RGB.hpp"
#include "../../Bus.hpp"
#include "../common.hpp"

#include "Bus_RGB.hpp"

#include <soc/gpio_reg.h>
#include <soc/gpio_periph.h>
#include "../../misc/common_function.hpp"
namespace lgfx
{
  inline namespace v1
  {
    //----------------------------------------------------------------------------
    void Panel_RGB::dumpFrameBuffer(int num_lines, int num_pixels_per_line) {
      Serial.println("Framebuffer-Dump:");
      for (int y = 0; y < num_lines; ++y) {
        Serial.printf("Line %d: ", y);
        uint16_t* line = (uint16_t*)_lines_buffer[y];
        for (int x = 0; x < num_pixels_per_line; ++x) {
          Serial.printf("%04X ", line[x]);
        }
        Serial.println();
      }
    }

    Panel_RGB::Panel_RGB(void)
    {
      _write_depth = color_depth_t::rgb565_2Byte;
      _read_depth = color_depth_t::rgb565_2Byte;
      // _write_depth = color_depth_t::rgb332_1Byte;
      // _read_depth = color_depth_t::rgb332_1Byte;
    }

    Panel_RGB::~Panel_RGB(void)
    {
      deinitFrameBuffer();
    }

    bool Panel_RGB::init(bool use_reset)
    {
      //Serial.printf("Panel_RGB::init use_reset: %d\n", use_reset);
      Serial.printf("Panel_RGB::init calling Panel_FrameBufferBase::init use_reset: %d\n", use_reset);
      if (!Panel_FrameBufferBase::init(use_reset))
      {
        Serial.printf("Panel_RGB::init failed Panel_FrameBufferBase::init\n");
        return false;
      }
      Serial.printf("Panel_RGB::init Panel_FrameBufferBase::init succeeded\n");

      auto h = _cfg.panel_height;

      auto frame_buffer_ = _bus->getDMABuffer(0);
      size_t lineArray_size = h * sizeof(void *);
      uint8_t **lineArray = (uint8_t **)heap_alloc_dma(lineArray_size);

      if (lineArray)
      {
        _lines_buffer = lineArray;
        memset(lineArray, 0, lineArray_size);

        Serial.printf("Panel_RGB::init h: %d, panel_width: %d, bits: %d\n", h, _cfg.panel_width, _write_bits);
        Serial.printf("Panel_RGB::init _write_depth: %d, _read_depth: %d\n", (int)_write_depth, (int)_read_depth);

        uint8_t bits = _write_bits;
        int w = (_cfg.panel_width + 3) & ~3;
        if (frame_buffer_)
        {
          auto fb = frame_buffer_;
          for (int i = 0; i < h; ++i)
          {
            lineArray[i] = fb;
            fb += w * bits >> 3;
          }
          
          int32_t pin_cs = _config_detail.pin_cs;
          if (pin_cs >= 0)
          {
            //Serial.printf("Panel_RGB::init pin_cs: %d\n", pin_cs);
            //delay(10); // Wait for a while before setting CS pin
            //Serial.printf("Panel_RGB::init setting pin %d to hi\n", pin_cs);
            //delay(10); // Wait for a while before setting CS pin
            lgfx::gpio_hi(pin_cs);
            //Serial.printf("Panel_RGB::init setting pin %d to output\n", pin_cs);
            lgfx::pinMode(pin_cs, pin_mode_t::output);
            //delay(10); // Wait for a while before setting CS pin
          }
          Serial.printf("Panel_RGB::init succeeded with frame_buffer_ allocated\n");
          return true;
        }
        heap_free(lineArray);
      }

      Serial.printf("Panel_RGB::init failed to allocate frame_buffer_\n");
      return false;
    }
    /*
      static inline uint8_t* sub_heap_alloc(bool flg_psram, size_t size)
      {
        uint8_t* res = nullptr;
        if (flg_psram) { res = (uint8_t*)heap_alloc_psram(size); }
        if (res == nullptr)
        {
          res = (uint8_t*)heap_alloc_dma(size);
        }
        if (res) { memset(res, 0, size); }
        return (uint8_t*)res;
      }
    */

    bool Panel_RGB::initFrameBuffer(uint_fast16_t w, uint_fast16_t h, color_depth_t depth, uint8_t chunk_lines, uint8_t use_psram)
    {
      Serial.printf("Panel_RGB::initFrameBuffer w: %d, h: %d, depth: %d, chunk_lines: %d, use_psram: %d\n", w, h, (int)depth, chunk_lines, use_psram);
      size_t lineArray_size = h * sizeof(void *);
      // ESP_LOGE("DEBUG","height:%d", h);
      uint8_t **lineArray = (uint8_t **)heap_alloc_dma(lineArray_size);
      if (lineArray)
      {
        memset(lineArray, 0, lineArray_size);

        uint8_t bits = (depth & color_depth_t::bit_mask);
        w = (w + 3) & ~3;
        // Temporary implementation. Allocate the buffer for the entire screen at once.
        // ToDo : Allocate in parts
        _frame_buffer = (uint8_t *)heap_alloc_psram((w * bits >> 3) * h);
        if (_frame_buffer)
        {
          _lines_buffer = lineArray;
          auto fb = _frame_buffer;
          for (int i = 0; i < h; ++i)
          {
            lineArray[i] = fb;
            fb += w * bits >> 3;
          }
          Serial.printf("Panel_RGB::initFrameBuffer succeeded with frame_buffer_ allocated\n");
          return true;
        }
        heap_free(lineArray);
      }
      Serial.printf("Panel_RGB::initFrameBuffer failed to allocate frame_buffer_\n");
      return false;
    }

    void Panel_RGB::deinitFrameBuffer(void)
    {
      Serial.printf("Panel_RGB::deinitFrameBuffer\n");
      if (_frame_buffer)
      {
        heap_free(_frame_buffer);
        _frame_buffer = nullptr;
      }

      if (_lines_buffer)
      {
        heap_free(_lines_buffer);
        _lines_buffer = nullptr;
      }
      Serial.printf("Panel_RGB::deinitFrameBuffer done\n");
    }

    static void _write_swspi(uint32_t data, uint8_t bits, uint8_t pin_sclk, uint8_t pin_mosi)
    {
      uint_fast8_t mask = 1 << (bits - 1);
      do
      {
        gpio_lo(pin_sclk);
        if (data & mask)
        {
          gpio_hi(pin_mosi);
        }
        else
        {
          gpio_lo(pin_mosi);
        }
        gpio_hi(pin_sclk);
      } while (mask >>= 1);
    }

    void Panel_RGB::writeCommand(uint32_t data, uint_fast8_t len)
    {
      // Serial.printf("Panel_RGB::writeCommand data: %08x, len: %d\n", data, len);
      do
      {
        // printf("CMD: %02x \n", data & 0xFF);
        _write_swspi(data & 0xFF, 9, _config_detail.pin_sclk, _config_detail.pin_mosi);
        data >>= 8;
      } while (--len);
    }

    void Panel_RGB::writeData(uint32_t data, uint_fast8_t len)
    {
      // Serial.printf("Panel_RGB::writeData data: %08x, len: %d\n", data, len);
      do
      {
        // printf("DAT: %02x \n", data & 0xFF);
        _write_swspi(data | 0x100, 9, _config_detail.pin_sclk, _config_detail.pin_mosi);
        data >>= 8;
      } while (--len);
    }

    //----------------------------------------------------------------------------
    /* Panel ST7701 base initialization */
    bool Panel_ST7701_Base::init(bool use_reset)
    {
      if (!Panel_RGB::init(use_reset))
      {
        return false;
      }

      int32_t pin_mosi = _config_detail.pin_mosi;
      int32_t pin_sclk = _config_detail.pin_sclk;
      if (pin_mosi >= 0 && pin_sclk >= 0)
      {
        lgfx::gpio::pin_backup_t backup_pins[] = {(gpio_num_t)pin_mosi, (gpio_num_t)pin_sclk};

        lgfx::gpio_lo(pin_mosi);
        lgfx::pinMode(pin_mosi, pin_mode_t::output);
        lgfx::gpio_lo(pin_sclk);
        lgfx::pinMode(pin_sclk, pin_mode_t::output);

        int32_t pin_cs = _config_detail.pin_cs;
        lgfx::gpio_lo(pin_cs);

        writeCommand(0xFF, 1);
        writeData(0x77, 1);
        writeData(0x01, 1);
        writeData(0x00, 2);
        writeData(0x10, 1);

        // 0xC0 : LNSET : Display Line Setting
        writeCommand(0xC0, 1);
        uint32_t line1 = (_cfg.panel_height >> 3) + 1;
        uint32_t line2 = (_cfg.panel_height >> 1) & 3;
        writeData(line1 + (line2 ? 0x80 : 0x00), 1);
        writeData(line2, 1);

        // 0xC3 : RGBCTRL
        auto cfg = ((Bus_RGB *)_bus)->config();
        writeCommand(0xC3, 1);
        uint32_t rgbctrl = 0;
        if (cfg.de_idle_high)
          rgbctrl += 0x01;
        if (cfg.pclk_idle_high)
          rgbctrl += 0x02;
        if (!cfg.hsync_polarity)
          rgbctrl += 0x04;
        if (!cfg.vsync_polarity)
          rgbctrl += 0x08;
        writeData(rgbctrl, 1);
        writeData(0x10, 1);
        writeData(0x08, 1);

        for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
        {
          command_list(cmds);
        }

        lgfx::gpio_hi(pin_cs);
        for (auto &bup : backup_pins)
        {
          bup.restore();
        }
      }

      return true;
    }

    const uint8_t *Panel_ST7701::getInitCommands(uint8_t listno) const
    {
      static constexpr const uint8_t list0[] =
          {
              // Command2 BK0 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

              0xC1, 2, 0x0D, 0x02,
              0xC2, 2, 0x31, 0x05,
              0xCD, 1, 0x08,

              // Positive Voltage Gamma Control
              0xB0, 16, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18,

              // Negative Voltage Gamma Control
              0xB1, 16, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18,

              // Command2 BK1 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

              0xB0, 1, 0x60, // Vop=4.7375v
              0xB1, 1, 0x32, // VCOM=32
              0xB2, 1, 0x07, // VGH=15v
              0xB3, 1, 0x80,
              0xB5, 1, 0x49, // VGL=-10.17v
              0xB7, 1, 0x85,
              0xB8, 1, 0x21, // AVDD=6.6 & AVCL=-4.6
              0xC1, 1, 0x78,
              0xC2, 1, 0x78,

              0xE0, 3, 0x00, 0x1B, 0x02,

              0xE1, 11, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44,
              0xE2, 12, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00,

              0xE3, 4, 0x00, 0x00, 0x11, 0x11,
              0xE4, 2, 0x44, 0x44,

              0xE5, 16, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0,

              0xE6, 4, 0x00, 0x00, 0x11, 0x11,

              0xE7, 2, 0x44, 0x44,

              0xE8, 16, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0,

              0xEB, 7, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40,
              0xEC, 2, 0x3C, 0x00,
              0xED, 16, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA,

              //-----------VAP & VAN---------------
              // Command2 BK3 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x13,

              0xE5, 1, 0xE4,

              // Command2 BK0 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x00,

              0x21, 0, // 0x20 normal, 0x21 IPS
              0x3A, 1, 0x60, // 0x70 RGB888, 0x60 RGB666, 0x50 RGB565

              0x11, CMD_INIT_DELAY, 120, // Sleep Out

              0x29, 0, // Display On

              0xFF, 0xFF,
          };
      switch (listno)
      {
      case 0:
        return list0;
      default:
        return nullptr;
      }
    }

    const uint8_t *Panel_ST7701_guition_esp32_4848S040::getInitCommands(uint8_t listno) const
    {
      static constexpr const uint8_t list0[] =
          {
              // Command2 BK0 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x10,

              0xC0, 2, 0x3B, 0x00, // added by manufacture
              0xC1, 2, 0x0D, 0x02,
              0xC2, 2, 0x31, 0x05,
              0xCD, 1, 0x00, // changed by manufacture

              // Positive Voltage Gamma Control
              0xB0, 16, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18,

              // Negative Voltage Gamma Control
              0xB1, 16, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18,

              // Command2 BK1 SEL
              0xFF, 5, 0x77, 0x01, 0x00, 0x00, 0x11,

              0xB0, 1, 0x60, // Vop=4.7375v
              0xB1, 1, 0x32, // VCOM=32
              0xB2, 1, 0x11,

              0xB0, 1, 0x60, // Vop=4.7375v
              0xB1, 1, 0x32, // VCOM=32
              0xB2, 1, 0x07, // VGH=15v
              0xB3, 1, 0x80,
              0xB5, 1, 0x49, // VGL=-10.17v
              0xB7, 1, 0x85,
              0xB8, 1, 0x21, // AVDD=6.6 & AVCL=-4.6
              0xC1,
              1,
              0x78,
              0xC2,
              1,
              0x78,

              0xE0,
              3,
              0x00,
              0x1B,
              0x02,

              0xE1,
              11,
              0x08,
              0xA0,
              0x00,
              0x00,
              0x07,
              0xA0,
              0x00,
              0x00,
              0x00,
              0x44,
              0x44,
              0xE2,
              12,
              0x11,
              0x11,
              0x44,
              0x44,
              0xED,
              0xA0,
              0x00,
              0x00,
              0xEC,
              0xA0,
              0x00,
              0x00,

              0xE3,
              4,
              0x00,
              0x00,
              0x11,
              0x11,
              0xE4,
              2,
              0x44,
              0x44,

              0xE5,
              16,
              0x0A,
              0xE9,
              0xD8,
              0xA0,
              0x0C,
              0xEB,
              0xD8,
              0xA0,
              0x0E,
              0xED,
              0xD8,
              0xA0,
              0x10,
              0xEF,
              0xD8,
              0xA0,

              0xE6,
              4,
              0x00,
              0x00,
              0x11,
              0x11,

              0xE7,
              2,
              0x44,
              0x44,

              0xE8,
              16,
              0x09,
              0xE8,
              0xD8,
              0xA0,
              0x0B,
              0xEA,
              0xD8,
              0xA0,
              0x0D,
              0xEC,
              0xD8,
              0xA0,
              0x0F,
              0xEE,
              0xD8,
              0xA0,

              0xEB,
              7,
              0x02,
              0x00,
              0xE4,
              0xE4,
              0x88,
              0x00,
              0x40,
              0xEC,
              2,
              0x3C,
              0x00,
              0xED,
              16,
              0xAB,
              0x89,
              0x76,
              0x54,
              0x02,
              0xFF,
              0xFF,
              0xFF,
              0xFF,
              0xFF,
              0xFF,
              0x20,
              0x45,
              0x67,
              0x98,
              0xBA,

              //-----------VAP & VAN---------------
              // Command2 BK3 SEL
              0xFF,
              5,
              0x77,
              0x01,
              0x00,
              0x00,
              0x13,

              0xE5,
              1,
              0xE4,

              // Command2 BK0 SEL
              0xFF,
              5,
              0x77,
              0x01,
              0x00,
              0x00,
              0x00,

              // 0x21,  0, // 0x20 normal, 0x21 IPS // commented by manufacture
              0x3A, 1, 0x60, // 0x70 RGB888, 0x60 RGB666, 0x50 RGB565

              0x11, CMD_INIT_DELAY, 120, // Sleep Out

              0x29, 0, // Display On

              0xFF,
              0xFF,
          };
      switch (listno)
      {
      case 0:
        return list0;
      default:
        return nullptr;
      }
    }

    //----------------------------------------------------------------------------

    const uint8_t *Panel_GC9503::getInitCommands(uint8_t listno) const
    {
      static constexpr const uint8_t list0[] = 
      {
        0xF0, 5, 0x55, 0xAA, 0x52, 0x08, 0x00, // Command Page Select (Page 0)
        0xF6, 2, 0x5A, 0x87,                   // Interface/Power Control
        0xC1, 1, 0x3F,                         // Display Control (Source Output, Gate Output, Scan Direction etc.)
        0xCD, 1, 0x25,                         // Source Driver Control
        0xC9, 1, 0x10,                         // Source Driver Control
        0xF8, 1, 0x8A,                         // Power Control
        0xAC, 1, 0x45,                         // VCOM Setting (Voltage)
        0xA7, 1, 0x47,                         // Power Control
        0xA0, 1, 0xCC,                         // CHP_CTL5 (Charge Pump, affects voltage)
        0x86, 4, 0x99, 0xA3, 0xA3, 0x31,       // CHP_CTL6 (Charge Pump, affects voltage)
        0xFA, 4, 0x08, 0x08, 0x00, 0x04,       // Power Control
        0xA3, 1, 0x6E,                         // Power Control

        0xFD, 3, 0x28, 0x3C, 0x00,             // Panel Timing Control

        //0x71,
        //0x72,
        //0x73,
        //0x97,
        //0x83,

        0x9A, 1, 0x4A,                         // Unknown, probably Power/Timing
        0x9B, 1, 0x22,                         // Unknown, probably Power/Timing

        0x82, 2, 0x00, 0x00,                   // Unknown, probably Power/Timing
        0x80, 1, 0x54,                         // Unknown, probably Power/Timing
        
        0xB1, 1, 0x13, // Display Function Control
        // 0x33 = 0b 00110011 -> BGR-Mode, scan direction inverted
        //      Bits 01234567
        // 0x10 = 0b 00010000 -> RGB-Mode, scan direction normal
        // 0x13 = 0b 00010011 -> RGB-Mode, scan direction inverted
        // Bit 7 (1): SS - Source scan direction (1 = reverse, 0 = normal)
        // Bit 6 (1): GS - Gate scan direction (1 = reverse, 0 = normal)
        // Bit 3-5 (3)- Inversion Mode for Source Driver
        // Bit 2 (1) 0=RGB color info mode, 1=BGR color info mode
        // Bit 1 (1): reserved - 0
        // Bit 0 (1): 1 = enter into Display Inversion On mode, 0 = recover from Display Inversion On mode???
        // Bits 7,6,3,2 (0): Reserved or not used

        0x7A, 2, 0x0F, 0x13,                   // Unknown, probably Gamma/Timing
        0x7B, 2, 0x0F, 0x13,                   // Unknown, probably Gamma/Timing

        0x6D, 32, 0x0C, 0x03, 0x1E, 0x02, 0x08, 0x1A, 0x19, 0x03, 0x0D, 0x0E, 0x0F, 0x10, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x11, 0x12, 0x13, 0x14, 0x03, 0x19, 0x1A, 0x07, 0x01, 0x1E, 0x03, 0x0C, // Gamma Curve/Look-Up Table
        // 0x6D: Gamma Correction Table (32 values, mostly for positive/negative color gradients, affects color rendering)
        0x64, 16, 0x38, 0x04, 0x03, 0xC4, 0x03, 0x03, 0x38, 0x02, 0x03, 0xC6, 0x03, 0x03, 0x2C, 0x7A, 0x2C, 0x7A, // Gamma/Timing
        // 0x64: Gamma/Timing Register (16 values)
        //  - Typical for GC9503CV: Fine adjustment of the gamma curve, mostly for individual color channels (R, G, B) or gray levels
        //  - Values affect the voltage levels for the display of colors and transitions
        //  - The exact meaning of the bytes is manufacturer-specific, but often: [P1, P2, ..., Pn] for different segments of the gamma curve
        //  - See: GC9A01/ILI9488/ST7701 datasheets, open-source drivers
        0x65, 16, 0x38, 0x08, 0x03, 0xC0, 0x03, 0x03, 0x38, 0x06, 0x03, 0xC2, 0x03, 0x03, 0x2C, 0x7A, 0x2C, 0x7A, // Gamma/Timing
        0x66, 16, 0x83, 0xD0, 0x03, 0xC4, 0x03, 0x03, 0x83, 0xD0, 0x03, 0xC4, 0x03, 0x03, 0x2C, 0x7A, 0x2C, 0x7A, // Gamma/Timing
        
        0x60, 8, 0x38, 0x0C, 0x3C, 0x3C, 0x38, 0x0B, 0x3C, 0x3C, // Gamma/Timing
        // 0x60: Gamma/Timing Register (8 values)
        //  - Used for further fine adjustment of the gamma curve, mostly for certain segments or transitions
        //  - Values affect the voltage levels for brightness and contrast in certain areas of the gray or color rendering
        //  - The exact meaning of the bytes is manufacturer-specific, but often: [S1, S2, ..., S8] for segment points of the gamma curve
        //  - See: GC9A01/ILI9488/ST7701 datasheets, open-source drivers
        0x61, 8, 0xB3, 0xC4, 0x3C, 0x3C, 0xB3, 0xC4, 0x3C, 0x3C, // Gamma/Timing
        0x62, 8, 0xB3, 0xC4, 0x3C, 0x3C, 0xB3, 0xC4, 0x3C, 0x3C, // Gamma/Timing
        0x63, 8, 0x38, 0x0A, 0x3C, 0x3C, 0x38, 0x09, 0x3C, 0x3C, // Gamma/Timing
        
        //0x67, 16,
        0x68, 13, 0x77, 0x08, 0x0A, 0x08, 0x09, 0x00, 0x00, 0x18, 0x0A, 0x08, 0x09, 0x00, 0x00, // Gamma/Timing
        0x69, 7, 0x14, 0x22, 0x14, 0x22, 0x44, 0x22, 0x08, // Gamma/Timing

        0x6B, 1, 0x07, // Gamma/Timing

        //0x7A
        //0x7B

        0xD1, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        // 0xD1: Gamma/Timing Table (52 values)
        //  - Contains the gamma curve for the display (e.g. brightness and contrast progression)
        //  - Each value influences the voltage levels for the individual gray levels
        //  - Typical: Value pairs for different color channels or gray levels
        //  - The exact assignment is manufacturer-specific, but often:
        //    [V0, V1, V2, ..., Vn] for the voltage levels of the internal DACs
        //  - See: GC9A01/ILI9488/ST7701 datasheets, open-source drivers
        0xD2, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        0xD3, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        0xD4, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        0xD5, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        0xD6, 52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x22, 0x00, 0x2C, 0x00, 0x2E, 0x00, 0x56, 0x00, 0x58, 0x00, 0x7C, 0x00, 0x9A, 0x00, 0xCE, 0x00, 0xFA, 0x01, 0x4C, 0x01, 0x94, 0x01, 0x96, 0x01, 0xDA, 0x02, 0x32, 0x02, 0x76, 0x02, 0xCC, 0x03, 0x18, 0x03, 0x55, 0x03, 0x6B, 0x03, 0x9B, 0x03, 0xAC, 0x03, 0xB8, 0x03, 0xE0, 0x03, 0xFF, // Gamma/Timing
        
        0x11, CMD_INIT_DELAY, 200,              // Sleep Out, then 200ms delay
        // 0x3A, 1, 0x50,                          // Pixel Format Set (0x50 = RGB565)
        // 0x3A, 1, 0x60,                          // Pixel Format Set (0x60 = RGB666)
        0x3A, 1, 0x70,                          // Pixel Format Set (0x70 = RGB888)
        0x29, 0,                               // Display ON
        0xFF, 0xFF,                            // End of sequence
      };
      switch (listno)
      {
      case 0:
        return list0;
      default:
        return nullptr;
      }
    }

    bool Panel_GC9503::init(bool use_reset)
    {
      //Serial.printf("Panel_GC9503 init\n");
      //delay(10); // Wait for pin state to stabilize
      if (!Panel_RGB::init(use_reset))
      {
        //Serial.printf("Panel_GC9503 Panel_RGB init failed\n");
        return false;
      }
      //Serial.printf("Panel_GC9503 Panel_RGB init success\n");
      //delay(10); // Wait for pin state to stabilize

      int32_t pin_mosi = _config_detail.pin_mosi;
      int32_t pin_sclk = _config_detail.pin_sclk;
      //Serial.printf("Panel_GC9503 pin_mosi:%d, pin_sclk:%d\n", pin_mosi, pin_sclk);
      //delay(10); // Wait for pin state to stabilize
      if (pin_mosi >= 0 && pin_sclk >= 0)
      {
        //Serial.printf("Panel_GC9503 pin_mosi and pin_sclk are valid\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio::pin_backup_t backup_pins[] = {(gpio_num_t)pin_mosi, (gpio_num_t)pin_sclk};
        //Serial.printf("Panel_GC9503 backup_pins created\n");
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Panel_GC9503 setting pin modes\n");
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Panel_GC9503 setting pin modes\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_lo(pin_mosi);
        lgfx::pinMode(pin_mosi, pin_mode_t::output);
        lgfx::gpio_lo(pin_sclk);
        lgfx::pinMode(pin_sclk, pin_mode_t::output);

        //Serial.printf("Panel_GC9503 setting pin_cs\n");
        int32_t pin_cs = _config_detail.pin_cs;
        //Serial.printf("pin_cs: %d\n", pin_cs);
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Calling lgfx::gpio_lo LOW for pin_cs\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_lo(pin_cs);

        Serial.printf("Panel_GC9503::init - Get the init sequence!\n");
        //delay(10); // Wait for pin state to stabilize
        for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
        {
          command_list(cmds);
        }
        Serial.printf("Panel_GC9503::init - Panel init sequence written\n");

        //Serial.printf("Calling lgfx::gpio_hi HIGH for pin_cs\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_hi(pin_cs);
        for (auto &bup : backup_pins)
        {
          bup.restore();
        }
      }
      Serial.printf("Panel_GC9503 init completed\n");
      // Initialization successful
      return true;
    }

    //----------------------------------------------------------------------------

    const uint8_t *Panel_GC9503_Quantum_Clock::getInitCommands(uint8_t listno) const
    {
      static constexpr const uint8_t list0[] = 
      {

        0xF0, 5, 0x55,0xAA,0x52,0x08,0x00,         // Command Page Select (Page 0)
        0xF6, 2, 0x5A,0x87,                       // Interface/Power Control
        0xC1, 1, 0x3F,                           // Display Control (Source Output, Gate Output, Scan Direction etc.)
        0xCD, 1, 0x25,                           // Source Driver Control
        0xC9, 1, 0x10,                           // Source Driver Control
        0xF8, 1, 0x8A,                           // Power Control
        0xAC, 1, 0x45,                           // VCOM Setting (Voltage)
        0xA7, 1, 0x47,                           // Power Control
        0xA0, 1, 0xCC,                           // CHP_CTL5 (Charge Pump, affects voltage)
        0x86, 4, 0x99,0xA3,0xA3,0x31,            // CHP_CTL6 (Charge Pump, affects voltage)
        0xFA, 4, 0x08,0x08,0x00,0x04,            // Power Control
        0xA3, 1, 0x6E,                           // Power Control
        0xFD, 3, 0x28,0x3C,0x00,                 // Panel Timing Control
        0x9A, 1, 0x4a,                           // Unknown, probably Power/Timing
        0x9B, 1, 0x22,                           // Unknown, probably Power/Timing
        0x82, 2, 0x00,0x00,                      // Unknown, probably Power/Timing
        0x80, 1, 0x4a,                           // Unknown, probably Power/Timing
        0xB1, 1, 0x30,                        // Display Function Control (alternative value)
        
        // 0xB1, 1, 0x10,                           // Display Function Control
        // 0x33 = 0b 00110011 -> BGR-Mode, scan direction inverted
        //      Bits 01234567
        // 0x10 = 0b 00010000 -> RGB-Mode, scan direction normal
        // 0x13 = 0b 00010011 -> RGB-Mode, scan direction inverted
        // Bit 7 (1): SS - Source scan direction (1 = reverse, 0 = normal)
        // Bit 6 (1): GS - Gate scan direction (1 = reverse, 0 = normal)
        // Bit 3-5 (3)- Inversion Mode for Source Driver
        // Bit 2 (1) 0=RGB color info mode, 1=BGR color info mode
        // Bit 1 (1): reserved - 0
        // Bit 0 (1): 1 = enter into Display Inversion On mode, 0 = recover from Display Inversion On mode???
        // Bits 7,6,3,2 (0): Reserved or not used
        
        0x7A, 2, 0x0F,0x13,                      // Unknown, probably Gamma/Timing
        0x7B, 2, 0x0F,0x13,                      // Unknown, probably Gamma/Timing
        
        0x6D,32, 0x03,0x03,0x1e,0x02,0x08,0x1a,0x19,0x0d,0x11,0x12,0x13,0x14,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x0c,0x0b,0x0a,0x09,0x0d,0x19,0x1a,0x07,0x01,0x1E,0x03,0x03, // Gamma Correction Table (32 values for color gradients)
        0x64,16, 0x38,0x04,0x03,0xc4,0x03,0x03,0x38,0x02,0x03,0xc6,0x03,0x03,0x2C,0x7A,0x2C,0x7A, // Gamma/Timing Register (fine adjustment of gamma curve)
        0x65,16, 0x38,0x08,0x03,0xc0,0x03,0x03,0x38,0x06,0x03,0xc2,0x03,0x03,0x2C,0x7A,0x2C,0x7A, // Gamma/Timing Register (color channel adjustment)
        0x66,16, 0x83,0xd0,0x03,0xc4,0x03,0x03,0x83,0xd0,0x03,0xc4,0x03,0x03,0x2C,0x7A,0x2C,0x7A, // Gamma/Timing Register (voltage levels for transitions)
        
        0x60, 8, 0x38,0x0c,0x3c,0x3c,0x38,0x0b,0x3c,0x3c, // Gamma/Timing Register (segment points adjustment)
        0x61, 8, 0xb3,0xc4,0x3c,0x3c,0xb3,0xc4,0x3c,0x3c, // Gamma/Timing Register (brightness/contrast segments)
        0x62, 8, 0xb3,0xc4,0x3c,0x3c,0xb3,0xc4,0x3c,0x3c, // Gamma/Timing Register (gray level adjustment)
        0x63, 8, 0x38,0x0a,0x3c,0x3c,0x38,0x09,0x3c,0x3c, // Gamma/Timing Register (further gamma curve segments)
        
        0x68,13, 0x77,0x08,0x0a,0x08,0x09,0x00,0x00,0x18,0x0a,0x08,0x09,0x00,0x00, // Gamma/Timing Control (display timing parameters)
        
        0x69, 7, 0x14,0x22,0x14,0x22,0x44,0x22,0x08, // Gamma/Timing Control (additional timing settings)
        0x6B, 1, 0x07,                           // Gamma/Timing Control (final gamma adjustment)
        
        0xD1,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Positive Gamma Correction (52 values for voltage levels of internal DACs)
        0xD2,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Negative Gamma Correction (Red channel gamma curve)
        0xD3,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Gamma Correction Table (Green channel gamma curve)
        0xD4,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Gamma Correction Table (Blue channel gamma curve)
        0xD5,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Gamma Correction Table (additional color calibration)
        0xD6,52, 0x00,0x00,0x00,0x10,0x00,0x22,0x00,0x2c,0x00,0x2e,0x00,0x56,0x00,0x58,0x00,0x7c,0x00,0x9a,0x00,0xce,0x00,0xfa,0x01,0x4c,0x01,0x94,0x01,0x96,0x01,0xda,0x02,0x32,0x02,0x76,0x02,0xcc,0x03,0x18,0x03,0x55,0x03,0x6b,0x03,0x9b,0x03,0xac,0x03,0xb8,0x03,0xe0,0x03,0xFF, // Gamma Correction Table (final gamma curve adjustment)
        
        0x11, CMD_INIT_DELAY, 120,              // Sleep Out, then 200ms delay
        
        0x3A, 1, 0x50,                          // Pixel Format Set (0x50 = RGB565)
        // 0x3A, 1, 0x60,                          // Pixel Format Set (0x60 = RGB666)
        // 0x3A, 1, 0x70,                          // Pixel Format Set (0x70 = RGB888)
        
        //0x29, 0,                               // Display ON
        0x28, 0,                               // Display OFF
        0xFF, 0xFF,                            // End of sequence
      };
      switch (listno)
      {
      case 0:
        return list0;
      default:
        return nullptr;
      }
    }

    bool Panel_GC9503_Quantum_Clock::init(bool use_reset)
    {
      Serial.printf("Panel_GC9503_Quantum_Clock::init - BEGIN\n");
      //delay(10); // Wait for pin state to stabilize
      Serial.printf("Panel_GC9503_Quantum_Clock - Calling Panel_RGB::init!\n");
      if (!Panel_RGB::init(use_reset))
      {
        Serial.printf("Panel_GC9503_Quantum_Clock Panel_RGB init failed\n");
        return false;
      }
      Serial.printf("Panel_GC9503_Quantum_Clock Panel_RGB init success\n");
      //delay(10); // Wait for pin state to stabilize

      int32_t pin_mosi = _config_detail.pin_mosi;
      int32_t pin_sclk = _config_detail.pin_sclk;

      Serial.printf("Panel_GC9503_Quantum_Clock pin_mosi:%d, pin_sclk:%d\n", pin_mosi, pin_sclk);
      //delay(10); // Wait for pin state to stabilize
      if (pin_mosi >= 0 && pin_sclk >= 0)
      {
        //Serial.printf("Panel_GC9503_Quantum_Clock pin_mosi and pin_sclk are valid\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio::pin_backup_t backup_pins[] = {(gpio_num_t)pin_mosi, (gpio_num_t)pin_sclk};
        //Serial.printf("Panel_GC9503_Quantum_Clock backup_pins created\n");
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Panel_GC9503_Quantum_Clock setting pin modes\n");
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Calling lgfx::gpio_lo for pin_mosi and pin_sclk\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_lo(pin_mosi);
        lgfx::pinMode(pin_mosi, pin_mode_t::output);
        lgfx::gpio_lo(pin_sclk);
        lgfx::pinMode(pin_sclk, pin_mode_t::output);

        Serial.printf("Panel_GC9503_Quantum_Clock setting pin_cs\n");
        int32_t pin_cs = _config_detail.pin_cs;
        //Serial.printf("pin_cs: %d\n", pin_cs);
        //delay(10); // Wait for pin state to stabilize
        //Serial.printf("Calling lgfx::gpio_lo LOW for pin_cs\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_lo(pin_cs);
        
        Serial.printf("Panel_GC9503_Quantum_Clock::init - Get the init sequence!\n");
        //delay(10); // Wait for pin state to stabilize
        for (uint8_t i = 0; auto cmds = getInitCommands(i); i++)
        {
          command_list(cmds);
        }
        Serial.printf("Panel_GC9503_Quantum_Clock::init - Panel init sequence written\n");

        //Serial.printf("Calling lgfx::gpio_hi HIGH for pin_cs\n");
        //delay(10); // Wait for pin state to stabilize
        lgfx::gpio_hi(pin_cs);
        for (auto &bup : backup_pins)
        {
          bup.restore();
        }
      }
      Serial.printf("Panel_GC9503_Quantum_Clock init completed\n");
      // Initialization successful
      return true;
    }  // Panel_GC9503_Quantum_Clock::init








    void Panel_RGB::drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y, uint32_t rawcolor)
    {
      // Serial.printf("Panel_RGB::drawPixelPreclipped x:%d, y:%d, rawcolor:0x%08X\n", x, y, rawcolor);
      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + 1); }
        if (r & 2)                  { x = _width  - (x + 1); }
        if (r & 1) { std::swap(x, y); }
      }
      if (_write_bits >= 8)
      {
        _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x);
        _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x);
        _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y);
        _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y);

        size_t bytes = _write_bits >> 3;
        auto ptr = &_lines_buffer[y][x * bytes];
        uint32_t color = rawcolor;
        if (bytes == 2 && getSwapBytes()) {
          color = (rawcolor >> 8) | (rawcolor << 8);
        }
        memcpy(ptr, &color, bytes);
      }
      // Serial.printf("Panel_RGB::drawPixelPreclipped completed\n");
    }

    void Panel_RGB::writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, uint32_t rawcolor)
    {
      // Serial.printf("Panel_RGB::writeFillRectPreclipped x:%d, y:%d, w:%d, h:%d, rawcolor:0x%08X\n", x, y, w, h, rawcolor);
      // Serial.println("!!! Panel_RGB::writeFillRectPreclipped (SWAP-TEST) !!!");
      uint_fast8_t r = _internal_rotation;
      if (r)
      {
        if ((1u << r) & 0b10010110) { y = _height - (y + h); }
        if (r & 2)                  { x = _width  - (x + w); }
        if (r & 1) { std::swap(x, y);  std::swap(w, h); }
      }
      _range_mod.left   = std::min<int_fast16_t>(_range_mod.left  , x        );
      _range_mod.right  = std::max<int_fast16_t>(_range_mod.right , x + w - 1);
      _range_mod.top    = std::min<int_fast16_t>(_range_mod.top   , y        );
      _range_mod.bottom = std::max<int_fast16_t>(_range_mod.bottom, y + h - 1);

      h += y;
      if (_write_bits >= 8)
      {
        size_t bytes = _write_bits >> 3;
        uint32_t color = rawcolor;
        //Serial.printf("bytes: %d, swap: %d\n", bytes, getSwapBytes());
        if (bytes == 2) {
          //Serial.printf("Panel_RGB::writeFillRectPreclipped: getSwapBytes() = %d\n", (int)getSwapBytes());
        }
        if (bytes == 2 && getSwapBytes()) {
          color = (rawcolor >> 8) | (rawcolor << 8);
          //Serial.printf("Panel_RGB::writeFillRectPreclipped: SWAP! rawcolor=0x%04X, swapped=0x%04X\n", (uint16_t)rawcolor, (uint16_t)color);
        }
        do
        {
          auto ptr = &_lines_buffer[y][x * bytes];
          memset_multi(ptr, color, bytes, w);
        } while (++y < h);
      }
      // Serial.printf("Panel_RGB::writeFillRectPreclipped completed\n");
    }

    
  }  // namespace lgfx
}
#endif
#endif
