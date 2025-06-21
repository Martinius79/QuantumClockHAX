#include <Arduino.h>
#include <SPIFFS.h>

#include "quantumclock_config.h" // Your config file

LGFX lcd; // Your display instance

void setup() {
  Serial.begin(115200);
  delay(2000); // Wait for serial connection

  Serial.println("Starting display...");
  lcd.init();
  Serial.println("Display initialized");
  delay(1000); // Wait after display initialization

  Serial.println("Initializing SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed!");
  }

  Serial.println("Set color depth to 16 bit (RGB565)");
  lcd.setColorDepth(16); // Set color depth to 16 bit (RGB565)

  Serial.println("Set display rotation...");
  if (lcd.width() < lcd.height())
  {
    lcd.setRotation(lcd.getRotation() ^ 1);
  }

  // drawJpgFile() requires a JPEG file in SPIFFS
  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...0");
  lcd.drawJpgFile(SPIFFS, "/0.jpg", 0, 0); // Show image at top left
  delay(5000); // Wait to see the image

  lcd.fillScreen(TFT_BLACK);
  Serial.println("Draw JPEG image...3");
  lcd.drawJpgFile(SPIFFS, "/1.jpg", 0, 0); // Show image at top left
  delay(200); // Wait to draw the image
  
  Serial.println("Brightness: " + String(lcd.getBrightness())); // Query brightness (optional)  
  lcd.setBrightness(255); // Set brightness to maximum
  Serial.println("Brightness after setting: " + String(lcd.getBrightness())); // Query brightness after setting (optional)

  // lcd.setBrightness(117); // Set brightness to a medium value
  // Serial.println("Brightness after setting to 117: " + String(lcd.getBrightness())); // Query brightness after setting (optional)
  // delay(200); // Wait to see the color
  
  // Serial.println("Fill the screen with black...");
  // lcd.fillScreen(TFT_BLACK);
  // delay(1000); // Wait to see the color

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

  Serial.println("Setup complete.");
}

void loop() {
  Serial.println("In loop...");
  
  // Tryout - Fill the screen with different colors
  // Serial.println("Fill the screen with red...");
  // lcd.fillScreen(TFT_RED);  
  // delay(1000); // Wait to see the color
  // Serial.println("Fill the screen with green...");
  // lcd.fillScreen(TFT_GREEN); // Fill screen with green
  // delay(1000); // Wait to see the color
  // Serial.println("Fill the screen with blue...");
  // lcd.fillScreen(TFT_BLUE); // Fill screen with blue
  // delay(1000); // Wait to see the color
  // Serial.println("Fill the screen with black...");
  // lcd.fillScreen(TFT_BLACK); // Clear the screen
  // delay(1000); // Wait to see the screen
  // Serial.println("Fill the screen with white...");
  // lcd.fillScreen(TFT_WHITE); // Fill screen with white
  delay(1000); // Wait for the next iteration
}