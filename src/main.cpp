#include <Arduino.h>

// Libraries for display
#include "Arduino_Display.h"
#include "Arduino_ESP32SPI.h"
#include "Arduino_GFX.h"
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// pin mapping
#define TFT_CS 5
#define TFT_DC 27
#define TFT_RST 33
#define TFT_SCK 18
#define TFT_MOSI 23
#define TFT_MISO -1  // no data coming back

#define SD_CS 21
// SD_MISO 19
// for SCK, MOSI see TFT

String fileContents;

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO, VSPI /* spi_num */);
Arduino_GC9A01 *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, true /* IPS */);

void setup() {
  Serial.begin(115200);
  gfx->begin();

  // Initialize SD card
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;  // init failed
  }

   File file = SD.open("/file.txt");
   fileContents =  file.readString();
   file.close();
}

void loop() {
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setCursor(12, 120);
  gfx->print(fileContents);

  delay(1000);
}