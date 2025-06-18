#include <Arduino.h>
// #include <Wire.h>
#include "quantumclock_config.h" // Dein Dateiname
// #define LGFX_AUTODETECT
// #include <LGFX_AUTODETECT.hpp>

LGFX lcd; // Deine Display-Instanz

void setup() {
  Serial.begin(115200);
  delay(2000); // Wartezeit für die serielle Verbindung

  pinMode(LCD_SCL, OUTPUT); // CLK (MTCK)
  pinMode(LCD_CS, OUTPUT); // CS  (MTD0)

  digitalWrite(LCD_SCL, HIGH); // Setze CLK (MTCK) auf HIGH
  delay(100); // Kurze Verzögerung, um den Zustand zu stabilisieren
  digitalWrite(LCD_CS, HIGH); // Setze CS  (MTD0) auf
  delay(500); // Kurze Verzögerung, um den Zustand zu stabilisieren
  digitalWrite(LCD_SCL, LOW); // Setze CLK (MTCK) auf HIGH
  delay(100); // Kurze Verzögerung, um den Zustand zu stabilisieren
  digitalWrite(LCD_CS, LOW); // Setze CS  (MTD0) auf LOW  
  delay(100); // Kurze Verzögerung, um den Zustand zu stabilisieren


  // Serial.println("Starte I2C Bus...");
  // Wire.begin(GPIO_NUM_38, GPIO_NUM_39); // I2C Pins (SDA, SCL)
  // Serial.println("I2C Bus gestartet");

  // Serial.println("Suche nach I2C Geräten...");
  // byte error, address;
  // int nDevices = 0;

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

  Serial.println("Setze Farbtiefe auf 16 Bit (RGB565)");
  lcd.setColorDepth(16); // Setze die Farbtiefe auf 16 Bit (RGB565)
  
  //lcd.spiWrite(0x28); // Display ausschalten
  // lcd.powerSaveOn(); // Display in den Energiesparmodus versetzen
  // delay(5000); // Wartezeit für den Energiesparmodus
  // lcd.powerSaveOff(); // Display aus dem Energiesparmodus aufwecken
  // delay(1000); // Wartezeit für die Display-Aufweckung
  Serial.println("Display Einstellungen:");

  // lcd.setRotation(0); // Optional: Drehung des Displays


  Serial.println("Helligkeit: " + String(lcd.getBrightness())); // Helligkeit abfragen (optional)  
  lcd.setBrightness(255); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(200); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(0); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen auf 0: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(200); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(117); // Helligkeit auf einen mittleren Wert setzen
  Serial.println("Helligkeit nach Setzen auf 117: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  delay(200); // Wartezeit, um die Farbe zu sehen
  lcd.setBrightness(255); // Helligkeit auf Maximum setzen
  Serial.println("Helligkeit nach Setzen: " + String(lcd.getBrightness())); // Helligkeit nach dem Setzen abfragen (optional)
  
  Serial.println("Fülle den Bildschirm mit Schwarz...");
  lcd.fillScreen(TFT_BLACK);
  delay(1000); // Wartezeit, um die Farbe zu sehen

  Serial.println("Schreibe Text auf den Bildschirm...");
  lcd.setTextSize(8);
  lcd.setTextColor(TFT_WHITE);
  lcd.setCursor(10, 10);
  lcd.println("1");
  //lcd.writeCommand(0x2C);
//  lcd.writeData(0x32);
  // lcd.println("GC9503CV Display");
  // lcd.println("LovyanGFX Konfig");
  // lcd.println("Erfolgreich!");
  delay(5000); // Wartezeit, um die Nachricht zu sehen
  Serial.println("Fülle den Bildschirm mit Rot...");
  lcd.fillScreen(TFT_RED);
  delay(1000); // Wartezeit, um die Farbe zu sehen
    lcd.writeCommand(0x28); // Display einschalten
  delay (5000); // Wartezeit für die Display-Einschaltung

  lcd.writeCommand(0x29); // Display einschalten
  
  delay(1000); // Wartezeit für die Display-Einschaltung
}

void loop() {
  Serial.println("Im Loop...");
  Serial.println("Fülle den Bildschirm mit Rot...");
  lcd.fillScreen(TFT_RED);  
  delay(1000); // Wartezeit für die nächste Iteration
  Serial.println("Fülle den Bildschirm mit Grün...");
  lcd.fillScreen(TFT_GREEN); // Bildschirm grün füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  Serial.println("Fülle den Bildschirm mit Blau...");
  lcd.fillScreen(TFT_BLUE); // Bildschirm schwarz füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  Serial.println("Fülle den Bildschirm mit Schwarz...");
  lcd.fillScreen(TFT_BLACK); // Bildschirm löschen
  delay(1000); // Wartezeit, um den Bildschirm zu sehen
  Serial.println("Fülle den Bildschirm mit Weiß...");
  lcd.fillScreen(TFT_WHITE); // Bildschirm weiß füllen
  delay(1000); // Wartezeit, um die Farbe zu sehen
  // Dein Zeichencode hier
}