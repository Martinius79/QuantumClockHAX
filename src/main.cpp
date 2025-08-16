#include <Arduino.h>
#include <SPIFFS.h>

// #include "quantumclock_config.h" // Your config file
#include "tryout_config.h" // Your tryout config file

#include "lgfx/v1/platforms/esp32s3/Panel_RGB.hpp"

void printRAMSize();

// ----------------------------------------------------------------------------
// Auto timing scan configuration
// ----------------------------------------------------------------------------
#define ENABLE_TIMING_AUTOSCAN 1  // set to 0 to disable automatic porch/pulse scan
#if ENABLE_TIMING_AUTOSCAN
// Candidate values (edit to taste). Keep within reasonable ranges.
static const int HFP_LIST[] = { LCD_HSYNC_FRONT_PORCH }; // keep constant (or add more)
static const int HSW_LIST[] = { LCD_HSYNC_PULSE_WIDTH }; // example alternatives
static const int HBP_LIST[] = { LCD_HSYNC_BACK_PORCH }; // back porch sweep
static const int VFP_LIST[] = { LCD_VSYNC_FRONT_PORCH }; // keep constant
static const int VSW_LIST[] = { LCD_VSYNC_PULSE_WIDTH }; // vsync pulse variants
static const int VBP_LIST[] = { LCD_VSYNC_BACK_PORCH  }; // vertical back porch sweep

// Pixel clock candidates (Hz)
static const uint32_t PCLK_LIST[] = { 12000000 };
// Delay between reconfigurations (ms)
static const uint32_t AUTOSCAN_INTERVAL_MS = 100; // allow visual inspection

// Optional: show a solid color cycling for quick artifact spotting.
// Wenn du das initial geladene Bild stehen lassen willst, setze AUTOSCAN_DRAW_COLOR_FRAMES auf 0.
#define AUTOSCAN_DRAW_COLOR_FRAMES 0
#if AUTOSCAN_DRAW_COLOR_FRAMES
// static const uint16_t COLOR_LIST[] = { 0x0000, 0xF800, 0x07E0, 0x001F, 0xFFFF, 0xFFE0, 0x07FF, 0xF81F };
static const uint16_t COLOR_LIST[] = { 0xF800 };
#endif

static float calcFPS(uint32_t pclk, int hfp,int hsw,int hbp,int vfp,int vsw,int vbp,int active_w,int active_h) {
  uint32_t htotal = active_w + hfp + hbp + hsw;
  uint32_t vtotal = active_h + vfp + vbp + vsw;
  if (htotal == 0 || vtotal == 0) return 0.0f;
  double fps = (double)pclk / (double)(htotal * (double)vtotal);
  return (float)fps;
}
#endif

LGFX lcd; // Your display instance
const char* bit_names[16] = {
  "B0", "B1", "B2", "B3", "B4", // Blue (Bits 0-4)
  "G0", "G1", "G2", "G3", "G4", "G5", // Green (Bits 5-10)
  "R0", "R1", "R2", "R3", "R4"  // Red (Bits 11-15)
};

const char* bit_desc[16] = {
  "Blue, very dark (lowest bit)",    // B0
  "Blue, darker",                   // B1
  "Blue, medium",                   // B2
  "Blue, bright",                   // B3
  "Blue, maximum (strong blue)",    // B4
  "Green, very dark (lowest bit)",  // G0
  "Green, darker",                  // G1
  "Green, medium",                  // G2
  "Green, bright",                  // G3
  "Green, very bright",             // G4
  "Green, maximum (strong green)",  // G5
  "Red, very dark (lowest bit)",    // R0
  "Red, darker",                    // R1
  "Red, medium",                    // R2
  "Red, bright",                    // R3
  "Red, maximum (strong red)"       // R4
};


void setup() {
  Serial.begin(115200);
  delay(2000); // Wait for serial connection

  Serial.println("Starting display...");

  printRAMSize();

  auto data = (uint8_t*)ps_malloc(800111);
  if (!data) {
    Serial.println("ERROR: PSRAM allocation for framebuffer failed!");
  }
  Serial.println("SUCCESS: set frame buffer: " + String((uintptr_t)data, HEX));
  printRAMSize();
  // deallocate PSRAM
  free(data);
  Serial.println("PSRAM deallocated");
  printRAMSize();
  // --- GPIO test before lcd.init() ---
  // Serial.println("Starting GPIO test for display data lines...");
  // // List of all used GPIOs for data lines (d0-d15)
  // int test_gpios[16] = {
  //   12, 13, 14, 15, 16, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4 // Adjust to your current mapping if necessary!
  // };
  // for (int i = 0; i < 16; ++i) {
  //   int gpio = test_gpios[i];
  //   if (gpio < 0) continue; // Skip -1 or NC
  //   Serial.print("Set GPIO "); Serial.print(gpio); Serial.println(" HIGH");
  //   pinMode(gpio, OUTPUT);
  //   digitalWrite(gpio, HIGH);
  //   delay(200);
  //   Serial.print("Set GPIO "); Serial.print(gpio); Serial.println(" LOW");
  //   digitalWrite(gpio, LOW);
  //   delay(200);
  // }
  // Serial.println("GPIO test completed. Starting display init...");

  lcd.init();
  Serial.println("Display initialized");
  delay(1000); // Wait after display initialization
  printRAMSize();
  // Not implemented!
  // Serial.println("Activating Swap Bytes...");  
  // lcd.setSwapBytes(true); // Set byte order to big-endian (default is little-endian)

  // own implementation
  // Swap the bytes for the framebuffer
  // Serial.println("Swap the bytes for the framebuffer");
  // static_cast<lgfx::v1::Panel_GC9503_Quantum_Clock*>(lcd.getPanel())->setSwapBytes(true);
  
  Serial.println("Initializing SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed!");
  }

  // seems to also not implemented
  // Serial.println("Set color depth to 16 bit (RGB565). Should be the default anyway.");
  lcd.setColorDepth(16); // Set color depth to 16 bit (RGB565)

  Serial.println("Set display rotation...");
  if (lcd.width() < lcd.height())
  {
    Serial.println("Display is in portrait mode (w<h), setting rotation to 1...");
    lcd.setRotation(lcd.getRotation() ^ 1);
  }else
  {
    Serial.println("Display is in landscape mode (w>=h), setting rotation to 0...");
    lcd.setRotation(lcd.getRotation() ^ 0);
  }

  // drawJpgFile() requires a JPEG file in SPIFFS
  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw JPEG image...0");
  // lcd.drawJpgFile(SPIFFS, "/0.jpg", 0, 0); // Show image at top left
  // delay(5000); // Wait to see the image

  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw BMP image...24bit - 2");
  // lcd.drawBmpFile(SPIFFS, "/2.bmp", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image

  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw BMP image...16bit - 4");
  // lcd.drawBmpFile(SPIFFS, "/4.bmp", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw JPEG image...1");
  // lcd.drawJpgFile(SPIFFS, "/1.jpg", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image

  // // lcd.fillScreen(TFT_BLACK);
  // // Serial.println("Draw JPEG image...3.jpg");
  // // lcd.drawJpgFile(SPIFFS, "/3.jpg", 0, 0); // Show image at top left
  // // delay(5000); // Wait to draw the image
  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw JPEG image...5.jpg");
  // lcd.drawJpgFile(SPIFFS, "/5.jpg", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image

  // // lcd.fillScreen(TFT_BLACK);
  // // Serial.println("Draw JPEG image...6.jpg");
  // // lcd.drawJpgFile(SPIFFS, "/6.jpg", 0, 0); // Show image at top left
  // // delay(5000); // Wait to draw the image
  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw JPEG image...7.jpg");
  // lcd.drawJpgFile(SPIFFS, "/7.jpg", 0, 0); // Show image at top left
  // delay(1000); // Wait to draw the image

  // // lcd.fillScreen(TFT_BLACK);
  // // Serial.println("Draw JPEG image...8.jpg");
  // // lcd.drawJpgFile(SPIFFS, "/8.jpg", 0, 0); // Show image at top left
  // // delay(5000); // Wait to draw the image
  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw JPEG image...9.jpg");
  // lcd.drawJpgFile(SPIFFS, "/9.jpg", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image
 
  // Serial.println("Brightness: " + String(lcd.getBrightness())); // Query brightness (optional)  
  // lcd.setBrightness(255); // Set brightness to maximum
  // Serial.println("Brightness after setting: " + String(lcd.getBrightness())); // Query brightness after setting (optional)

  // lcd.setBrightness(117); // Set brightness to a medium value
  // Serial.println("Brightness after setting to 117: " + String(lcd.getBrightness())); // Query brightness after setting (optional)
  // delay(200); // Wait to see the color
  
  // Serial.println("Fill the screen with black...");
  // lcd.fillScreen(TFT_BLACK);
  // delay(1000); // Wait to see the color

  Serial.println("Write text to the display...");
  // lcd.setTextSize(2);
  // lcd.setTextColor(TFT_WHITE);
  // lcd.setCursor(300, 240);
  // lcd.println("Hello from the Quantum Clock!");
  // lcd.setCursor(380, 260);
  // lcd.println("GC9503CV Display");
  // lcd.setCursor(380, 280);
  // lcd.println("LovyanGFX Config");
  // lcd.setCursor(390, 300);
  // lcd.setTextSize(3);
  // lcd.setTextColor(TFT_GREEN);
  // lcd.println("SUCCESS!");
  
  
  
  // lcd.setTextSize(2);
  // lcd.setTextColor(TFT_WHITE);
  // lcd.setCursor(0, 0);
  // lcd.println("Hello from the Quantum Clock!");
  lcd.setCursor(20, 20);
  lcd.println("GC9503 Display");
  // lcd.setCursor(30, 20);
  // lcd.println("LovyanGFX Config");
  // lcd.setCursor(40, 40);
  // lcd.setTextSize(3);
  // lcd.setTextColor(TFT_GREEN);
  // lcd.println("SUCCESS!");
  // delay(2000); // Wait to see the text

  Serial.println("Setup complete.");

  // --- Example: runtime panel command usage (turn display off, then on) ---
  // auto* panel = static_cast<lgfx::v1::Panel_GC9503_Quantum_Clock*>(lcd.getPanel());
  // panel->displayOff();        // Send 0x28
  // delay(500);
  // panel->sleepIn();          // Optional: enter sleep (0x10)
  // delay(1000);
  // panel->sleepOut();         // Exit sleep (0x11, includes internal delay)
  // panel->displayOn();        // 0x29
  // Serial.println("Display cycled OFF->SLEEP->ON");

  // --- Optional: Timing sweep example (commented) ---
  // This demonstrates how to adjust porches at runtime without full re-init.
  // Uncomment to try a small sweep.

  // auto* bus = static_cast<lgfx::v1::Bus_RGB*>(lcd.panel()->getBus());
  // uint32_t baseFreq = 12000000; // start PCLK
  // for (int hbp=20; hbp<=80; hbp+=10) {
  //   for (int vbp=8; vbp<=48; vbp+=8) {
  //     bus->reconfigureTimings(baseFreq,
  //       LCD_HSYNC_FRONT_PORCH, LCD_HSYNC_PULSE_WIDTH, hbp,
  //       LCD_VSYNC_FRONT_PORCH, LCD_VSYNC_PULSE_WIDTH, vbp);
  //     Serial.printf("Timing change -> hbp=%d vbp=%d\n", hbp, vbp);
  //     delay(500);
  //   }
  // }
}

//--------------------------------------------------------------------------------------------------------------------------


void loop() {
  // Serial.println("In loop...");
  
  // --- Color cycle commented out ---

  // Serial.println("Fill the screen with red...");  
  // lcd.fillScreen(TFT_RED);
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with green...");
  // lcd.fillScreen(TFT_GREEN); // Fill screen with green
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with blue...");
  // lcd.fillScreen(TFT_BLUE); // Fill screen with blue
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with black...");
  // lcd.fillScreen(TFT_BLACK); // Clear the screen
  // delay(2000); // Wait to see the screen

  // Serial.println("Fill the screen with white...");
  // lcd.fillScreen(TFT_WHITE); // Fill screen with white
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with yellow...");
  // lcd.fillScreen(TFT_YELLOW); // Fill screen with yellow
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with cyan...");
  // lcd.setTextColor(TFT_CYAN); // Set text color to cyan
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with magenta...");
  // lcd.fillScreen(TFT_MAGENTA); // Fill screen with magenta
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with dark gray...");  
  // lcd.fillScreen(TFT_DARKGRAY); // Fill screen with dark gray
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with orange...");
  // lcd.fillScreen(TFT_ORANGE); // Fill screen with orange
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with purple...");
  // lcd.fillScreen(TFT_PURPLE); // Fill screen with purple
  // delay(2000); // Wait to see the color

  // Serial.println("Fill the screen with pink...");  
  // lcd.fillScreen(TFT_PINK); // Fill screen with pink
  // delay(2000); // Wait to see the color

  // Avoid long blocking delay when autoscan active, else VSYNC stats will always show frames=1.
#if !ENABLE_TIMING_AUTOSCAN
  delay(2000);
#endif

  // --- Bit pattern test for RGB565 data lines ---
  // for (uint8_t bit = 0; bit < 16; ++bit) {
  //     uint16_t color = 1 << bit;
  //     Serial.print("Bit pattern test: Only bit ");
  //     Serial.print(bit);
  //     Serial.print(" set (0x");
  //     Serial.print(color, HEX);
  //     Serial.print(") - ");
  //     Serial.print(bit_names[bit]);
  //     Serial.print(" - ");
  //     Serial.println(bit_desc[bit]);
  //     lcd.fillScreen(color);
  //     auto* panel = static_cast<lgfx::v1::Panel_GC9503_Quantum_Clock*>(lcd.getPanel());
  //     panel->dumpFrameBuffer(2, 8);
  //     delay(300); // Time to observe
  // }
  // Serial.println("Bit pattern test completed.");
  // delay(3000);

  // --- Test for "full" colors (only one color channel full, rest 0) ---
  // struct {
  //   const char* name;
  //   uint16_t color;
  // } color_tests[] = {
  //   {"Full Red (0xF800)", 0xF800},
  //   {"Full Green (0x07E0)", 0x07E0},
  //   {"Full Blue (0x001F)", 0x001F},
  //   {"White (0xFFFF)", 0xFFFF},
  //   {"Black (0x0000)", 0x0000},
  //   {"Yellow (0xFFE0)", 0xFFE0},
  //   {"Cyan (0x07FF)", 0x07FF},
  //   {"Magenta (0xF81F)", 0xF81F},
  // };
  // for (auto& t : color_tests) {
  //   Serial.print("Fill with color: ");
  //   Serial.print(t.name);
  //   Serial.print(" (0x");
  //   Serial.print(t.color, HEX);
  //   Serial.println(")");
  //   lcd.fillScreen(t.color);
  //   delay(2000);
  // }
  // Serial.println("Color test completed.");
  // delay(3000);

#if ENABLE_TIMING_AUTOSCAN
  static uint32_t last_ms = 0;          // last time we changed timings
  static size_t idxPCLK = 0, idxHFP = 0, idxHSW = 0, idxHBP = 0, idxVFP = 0, idxVSW = 0, idxVBP = 0, idxCOLOR = 0;
  uint32_t now = millis();
  if (now - last_ms >= AUTOSCAN_INTERVAL_MS) {
    last_ms = now;
    // Current candidates
    uint32_t pclk = PCLK_LIST[idxPCLK];
    int hfp = HFP_LIST[idxHFP];
    int hsw = HSW_LIST[idxHSW];
    int hbp = HBP_LIST[idxHBP];
    int vfp = VFP_LIST[idxVFP];
    int vsw = VSW_LIST[idxVSW];
    int vbp = VBP_LIST[idxVBP];

  auto* bus = static_cast<lgfx::v1::Bus_RGB*>(lcd.panel()->getBus());
  bool ok = bus->reconfigureTimings(pclk, hfp, hsw, hbp, vfp, vsw, vbp);
    float fps = calcFPS(pclk, hfp, hsw, hbp, vfp, vsw, vbp, lcd.width(), lcd.height());
    #if AUTOSCAN_DRAW_COLOR_FRAMES
      uint16_t color = COLOR_LIST[idxCOLOR];
      lcd.fillScreen(color);
      Serial.printf("[AUTO-TIMING] ok=%d pclk=%lu H(fp/pw/bp)=(%d/%d/%d) V(fp/pw/bp)=(%d/%d/%d) fps~%.2f color=0x%04X\n",
                    (int)ok, (unsigned long)pclk, hfp, hsw, hbp, vfp, vsw, vbp, fps, color);
      idxCOLOR = (idxCOLOR + 1) % (sizeof(COLOR_LIST)/sizeof(COLOR_LIST[0]));
    #else
      // Keine Bildschirm-Neuzeichnung: vorhandenes Bild bleibt erhalten.
      Serial.printf("[AUTO-TIMING] ok=%d pclk=%lu H(fp/pw/bp)=(%d/%d/%d) V(fp/pw/bp)=(%d/%d/%d) fps~%.2f (no redraw)\n",
                    (int)ok, (unsigned long)pclk, hfp, hsw, hbp, vfp, vsw, vbp, fps);
    #endif

    // Nested index increment logic
    if (++idxVBP >= (sizeof(VBP_LIST)/sizeof(VBP_LIST[0]))) { idxVBP = 0; if (++idxVSW >= (sizeof(VSW_LIST)/sizeof(VSW_LIST[0]))) { idxVSW = 0; if (++idxVFP >= (sizeof(VFP_LIST)/sizeof(VFP_LIST[0]))) { idxVFP = 0; if (++idxHBP >= (sizeof(HBP_LIST)/sizeof(HBP_LIST[0]))) { idxHBP = 0; if (++idxHSW >= (sizeof(HSW_LIST)/sizeof(HSW_LIST[0]))) { idxHSW = 0; if (++idxHFP >= (sizeof(HFP_LIST)/sizeof(HFP_LIST[0]))) { idxHFP = 0; if (++idxPCLK >= (sizeof(PCLK_LIST)/sizeof(PCLK_LIST[0]))) { idxPCLK = 0; Serial.println("[AUTO-TIMING] Completed full sweep -> restarting."); } } } } } } }
  }
  // (VSYNC statistics removed — reverted state)
#endif
}

void printRAMSize() {

// #ifdef DEBUG_OUTPUT_RAM
  Serial.println("ESP32 RAM Info:");  
  Serial.printf("Total RAM: %u bytes\n", ESP.getHeapSize());
  Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
  // Serial.printf("Free Sketch: %u bytes\n", ESP.getFreeSketchSpace());
  Serial.printf("PSRAM Size: %u bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
// #endif
}