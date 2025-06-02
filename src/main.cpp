#include <Arduino.h>
#include "lvgl.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"

// Auflösung
#define LCD_HRES 960
#define LCD_VRES 360

// Pin-Zuordnung
#define PIN_DE      6
#define PIN_VSYNC   5
#define PIN_HSYNC   4
#define PIN_PCLK    7
#define PIN_DISP_EN 10

static const int rgb_pins[24] = {
  8, 3, 46, 9, 1, 2, 42, 41,     // R0-R7
  15,16,17,18,21,47,48,45,      // G0-G7
  40,39,38,37,36,35,34,33       // B0-B7
};

esp_lcd_panel_handle_t panel = NULL;

void lvgl_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_map) {
  esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(PIN_DISP_EN, OUTPUT);
  digitalWrite(PIN_DISP_EN, HIGH);

  // RGB Panel Config
esp_lcd_rgb_panel_config_t panel_config = {
    .clk_src = LCD_CLK_SRC_PLL160M,
    .timings = {
        .pclk_hz = 12 * 1000 * 1000,
        .h_res = LCD_HRES,
        .v_res = LCD_VRES,
        .hsync_pulse_width = 1,
        .hsync_back_porch = 20,
        .hsync_front_porch = 40,
        .vsync_pulse_width = 1,
        .vsync_back_porch = 8,
        .vsync_front_porch = 8,
        .flags = {
            .hsync_idle_low = false,
            .vsync_idle_low = false,
            .de_idle_high = true,
            .pclk_active_neg = true,
            .pclk_idle_high = false
        }
    },
    .data_width = 24,
    .sram_trans_align = 64,
    .psram_trans_align = 64,
    .hsync_gpio_num = PIN_HSYNC,
    .vsync_gpio_num = PIN_VSYNC,
    .de_gpio_num = PIN_DE,
    .pclk_gpio_num = PIN_PCLK,
    .data_gpio_nums = {
        8, 3, 46, 9, 1, 2, 42, 41,
        15, 16, 17, 18, 21, 47, 48, 45,
        40, 39, 38, 37, 36, 35, 34, 33
    },
    .disp_gpio_num = PIN_DISP_EN,
    .on_frame_trans_done = NULL,
    .user_ctx = NULL,
    .flags = {
        .disp_active_low = 0,
        .relax_on_idle = 0,
        .fb_in_psram = 0
    }
};

  ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel));
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

  // Init LVGL
  lv_init();
  static lv_color_t *buf = (lv_color_t *)heap_caps_malloc(LCD_HRES * LCD_VRES * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  static lv_disp_draw_buf_t draw_buf;
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_HRES * LCD_VRES);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_HRES;
  disp_drv.ver_res = LCD_VRES;
  disp_drv.flush_cb = lvgl_flush_cb;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Testobjekt
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "GC9503V ready");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop() {
  lv_timer_handler();
  delay(5);
}