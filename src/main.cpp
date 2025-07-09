#include <Arduino.h>
#include <SPIFFS.h>

#include "quantumclock_config.h" // Your config file

#include "lgfx/v1/platforms/esp32s3/Panel_RGB.hpp"

LGFX lcd; // Your display instance
const char* bit_names[16] = {
  "B0", "B1", "B2", "B3", "B4", // Blau (Bits 0-4)
  "G0", "G1", "G2", "G3", "G4", "G5", // Grün (Bits 5-10)
  "R0", "R1", "R2", "R3", "R4"  // Rot (Bits 11-15)
};

const char* bit_desc[16] = {
  "Blau, sehr dunkel (niedrigstes Bit)",    // B0
  "Blau, dunkler",                          // B1
  "Blau, mittel",                           // B2
  "Blau, hell",                             // B3
  "Blau, maximal (kräftiges Blau)",         // B4
  "Grün, sehr dunkel (niedrigstes Bit)",    // G0
  "Grün, dunkler",                          // G1
  "Grün, mittel",                           // G2
  "Grün, hell",                             // G3
  "Grün, sehr hell",                        // G4
  "Grün, maximal (kräftiges Grün)",         // G5
  "Rot, sehr dunkel (niedrigstes Bit)",     // R0
  "Rot, dunkler",                           // R1
  "Rot, mittel",                            // R2
  "Rot, hell",                              // R3
  "Rot, maximal (kräftiges Rot)"            // R4
};


void setup() {
  Serial.begin(115200);
  delay(2000); // Wait for serial connection

  Serial.println("Starting display...");

  // --- GPIO-Test vor lcd.init() ---
  // Serial.println("Starte GPIO-Test für Display-Datenleitungen...");
  // // Liste aller verwendeten GPIOs für Datenleitungen (d0-d15)
  // int test_gpios[16] = {
  //   12, 13, 14, 15, 16, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4 // Passe ggf. an deine aktuelle Zuordnung an!
  // };
  // for (int i = 0; i < 16; ++i) {
  //   int gpio = test_gpios[i];
  //   if (gpio < 0) continue; // -1 oder NC überspringen
  //   Serial.print("Setze GPIO "); Serial.print(gpio); Serial.println(" HIGH");
  //   pinMode(gpio, OUTPUT);
  //   digitalWrite(gpio, HIGH);
  //   delay(200);
  //   Serial.print("Setze GPIO "); Serial.print(gpio); Serial.println(" LOW");
  //   digitalWrite(gpio, LOW);
  //   delay(200);
  // }
  // Serial.println("GPIO-Test abgeschlossen. Starte Display-Init...");

  lcd.init();
  Serial.println("Display initialized");
  delay(1000); // Wait after display initialization
  
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

  // Serial.println("Set display rotation...");
  if (lcd.width() < lcd.height())
  {
    lcd.setRotation(lcd.getRotation() ^ 1);
  }

  // drawJpgFile() requires a JPEG file in SPIFFS
  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...0");
  lcd.drawJpgFile(SPIFFS, "/0.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to see the image

  // lcd.fillScreen(TFT_BLACK);
  // Serial.println("Draw BMP image...24bit - 2");
  // lcd.drawBmpFile(SPIFFS, "/2.bmp", 0, 0); // Show image at top left
  // delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw BMP image...16bit - 4");
  lcd.drawBmpFile(SPIFFS, "/4.bmp", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...1");
  lcd.drawJpgFile(SPIFFS, "/1.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...3.jpg");
  lcd.drawJpgFile(SPIFFS, "/3.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image
  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...5.jpg");
  lcd.drawJpgFile(SPIFFS, "/5.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...6.jpg");
  lcd.drawJpgFile(SPIFFS, "/6.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image
  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...7.jpg");
  lcd.drawJpgFile(SPIFFS, "/7.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...8.jpg");
  lcd.drawJpgFile(SPIFFS, "/8.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image
  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...9.jpg");
  lcd.drawJpgFile(SPIFFS, "/9.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to draw the image
 
  // Serial.println("Brightness: " + String(lcd.getBrightness())); // Query brightness (optional)  
  // lcd.setBrightness(255); // Set brightness to maximum
  // Serial.println("Brightness after setting: " + String(lcd.getBrightness())); // Query brightness after setting (optional)

  // lcd.setBrightness(117); // Set brightness to a medium value
  // Serial.println("Brightness after setting to 117: " + String(lcd.getBrightness())); // Query brightness after setting (optional)
  // delay(200); // Wait to see the color
  
  Serial.println("Fill the screen with black...");
  lcd.fillScreen(TFT_BLACK);
  delay(1000); // Wait to see the color

  Serial.println("Write text to the display...");
  lcd.setTextSize(2);
  lcd.setTextColor(TFT_WHITE);
  lcd.setCursor(300, 270);
  lcd.println("Hello from the Quantum Clock!");
  lcd.setCursor(380, 290);
  lcd.println("GC9503CV Display");
  lcd.setCursor(380, 310);
  lcd.println("LovyanGFX Config");
  lcd.setCursor(380, 330);
  lcd.setTextSize(3);
  lcd.setTextColor(TFT_GREEN);
  lcd.println("SUCCESS!");
  delay(2000); // Wait to see the text

  Serial.println("Setup complete.");
}

void loop() {
  Serial.println("In loop...");
  
  // --- Farbdurchlauf auskommentiert ---

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

  delay(2000); // Wait for the next iteration

  // --- Bitmuster-Test für RGB565 Datenleitungen ---
  for (uint8_t bit = 0; bit < 16; ++bit) {
      uint16_t color = 1 << bit;
      Serial.print("Bitmuster-Test: Nur Bit ");
      Serial.print(bit);
      Serial.print(" gesetzt (0x");
      Serial.print(color, HEX);
      Serial.print(") - ");
      Serial.print(bit_names[bit]);
      Serial.print(" - ");
      Serial.println(bit_desc[bit]);
      lcd.fillScreen(color);
      auto* panel = static_cast<lgfx::v1::Panel_GC9503_Quantum_Clock*>(lcd.getPanel());
      panel->dumpFrameBuffer(2, 8);
      delay(300); // Zeit zum Beobachten
  }
  Serial.println("Bitmuster-Test abgeschlossen.");
//   // delay(3000);

  // --- Test für "volle" Farben (nur ein Farbkanal voll, Rest 0) ---
  struct {
    const char* name;
    uint16_t color;
  } color_tests[] = {
    {"Voll Rot (0xF800)", 0xF800},
    {"Voll Grün (0x07E0)", 0x07E0},
    {"Voll Blau (0x001F)", 0x001F},
    {"Weiß (0xFFFF)", 0xFFFF},
    {"Schwarz (0x0000)", 0x0000},
    {"Gelb (0xFFE0)", 0xFFE0},
    {"Cyan (0x07FF)", 0x07FF},
    {"Magenta (0xF81F)", 0xF81F},
  };
  for (auto& t : color_tests) {
    Serial.print("Fülle mit Farbe: ");
    Serial.print(t.name);
    Serial.print(" (0x");
    Serial.print(t.color, HEX);
    Serial.println(")");
    lcd.fillScreen(t.color);
    delay(2000);
  }
  Serial.println("Farbtest abgeschlossen.");
  // delay(3000);


}