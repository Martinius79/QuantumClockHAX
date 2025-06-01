#include <Arduino.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "lv_conf.h"
#include "lvgl.h"
// #include "esp_lvgl_port.h"

// Displayauflösung
#define LCD_HRES 960
#define LCD_VRES 360

// RGB-Pins
#define PIN_DE      6
#define PIN_VSYNC   5
#define PIN_HSYNC   4
#define PIN_PCLK    7
#define PIN_DISP_EN 10

// Datenleitungen RGB888 (nur Beispiel – bitte ggf. anpassen!)
int data_pins[] = {
  8, 3, 46, 9, 1, 2, 42, 41,     // R0–R7
  15, 16, 17, 18, 21, 47, 48, 45, // G0–G7
  40, 39, 38, 37, 36, 35, 34, 33  // B0–B7
};

esp_lcd_panel_handle_t panel_handle = nullptr;

void init_display() {
  // Enable GPIO
  pinMode(PIN_DISP_EN, OUTPUT);
  digitalWrite(PIN_DISP_EN, HIGH);

  esp_lcd_rgb_panel_config_t panel_config = {
    .data_width = 24,
    .psram_trans_align = 64,
    .num_fbs = 1,
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .disp_gpio_num = PIN_DISP_EN,
    .pclk_gpio_num = PIN_PCLK,
    .vsync_gpio_num = PIN_VSYNC,
    .hsync_gpio_num = PIN_HSYNC,
    .de_gpio_num = PIN_DE,
    .data_gpio_nums = data_pins,
    .timings = {
      .pclk_hz = 10 * 1000 * 1000, // 10 MHz (je nach Panel höher möglich)
      .h_res = LCD_HRES,
      .v_res = LCD_VRES,
      .hsync_back_porch = 20,
      .hsync_front_porch = 40,
      .hsync_pulse_width = 1,
      .vsync_back_porch = 8,
      .vsync_front_porch = 8,
      .vsync_pulse_width = 1,
      .flags = {
        .pclk_active_neg = true,
      }
    },
    .flags = {
      .fb_in_psram = true,
      .double_buffer = false,
    }
  };

  esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
  esp_lcd_panel_reset(panel_handle);
  esp_lcd_panel_init(panel_handle);
  esp_lcd_panel_disp_on_off(panel_handle, true);
}

void init_lvgl() {
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(LCD_HRES * LCD_VRES * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LCD_HRES * LCD_VRES);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_HRES;
  disp_drv.ver_res = LCD_VRES;
  disp_drv.flush_cb = [](lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
    lv_disp_flush_ready(disp);
  };
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

void setup() {
  Serial.begin(115200);
  init_display();
  init_lvgl();

  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello, GC9503V!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop() {
  lv_timer_handler();
  delay(5);
}
