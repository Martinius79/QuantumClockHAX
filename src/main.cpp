#include <Arduino.h>
// #include <Wire.h>
#include "quantumclock_config.h" // Dein Dateiname
// #define LGFX_AUTODETECT
// #include <LGFX_AUTODETECT.hpp>

LGFX lcd; // Deine Display-Instanz

void setup() {
  Serial.begin(115200);
  delay(2000); // Wartezeit für die serielle Verbindung

  // Serial.println("Starte I2C Bus...");
  // Wire.begin(GPIO_NUM_38, GPIO_NUM_39); // I2C Pins (SDA, SCL)
  // Serial.println("I2C Bus gestartet");
    
  Serial.println("Starte Display...");
  lcd.init();
  Serial.println("Display initialisiert");
  delay(1000); // Wartezeit für die Display-Initialisierung

  // byte error, address;
  // int nDevices = 0;

  // Serial.println("Suche nach I2C Geräten...");

  // for(address = 1; address < 127; address++ ) {
  //   Wire.beginTransmission(address);
  //   error = Wire.endTransmission();

  //   if (error == 0) {
  //     Serial.print("I2C Gerät gefunden bei Adresse 0x");
  //     if (address<16)
  //       Serial.print("0");
  //     Serial.print(address,HEX);
  //     Serial.println(" !");
  //     nDevices++;
  //   }
  // }
  // if (nDevices == 0)
  //   Serial.println("Keine I2C Geräte gefunden\n");
  // else
  //   Serial.println("Fertig\n");

  
  lcd.setRotation(0); // Optional: Drehung des Displays

  Serial.println("Fülle den Bildschirm mit Rot...");
  Serial.println("Helligkeit: " + String(lcd.getBrightness())); // Helligkeit abfragen (optional)  
  lcd.setBrightness(255); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(1000); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(0); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen auf 0: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(1000); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(117); // Helligkeit auf einen mittleren Wert setzen
  Serial.println("Helligkeit nach Setzen auf 117: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(1000); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(255); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)


  lcd.fillScreen(TFT_RED);
  delay(1000); // Wartezeit, um die Farbe zu sehen
}

void loop() {
  Serial.println("Im Loop...");
  lcd.fillScreen(TFT_RED);
  delay(1000); // Wartezeit für die nächste Iteration
  lcd.fillScreen(TFT_GREEN); // Bildschirm grün füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  lcd.fillScreen(TFT_BLUE); // Bildschirm schwarz füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  lcd.fillScreen(TFT_BLACK); // Bildschirm löschen
  delay(1000); // Wartezeit, um den Bildschirm zu sehen
  lcd.fillScreen(TFT_WHITE); // Bildschirm weiß füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  // Dein Zeichencode hier
}