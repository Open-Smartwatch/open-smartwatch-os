#include <M5Stack.h>
#include <HTTPClient.h>

#include <math.h>

#include "pngle.h"

#define WIFI_SSID "XXXXXXXX"
#define WIFI_PASS "XXXXXXXX"

void cls()
{
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
}

// ===================================================
// pngle example for m5stack
// ===================================================

double g_scale = 1.0;
void pngle_on_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
  uint16_t color = (rgba[0] << 8 & 0xf800) | (rgba[1] << 3 & 0x07e0) | (rgba[2] >> 3 & 0x001f);

  if (rgba[3]) {
    x = ceil(x * g_scale);
    y = ceil(y * g_scale);
    w = ceil(w * g_scale);
    h = ceil(h * g_scale);
    M5.Lcd.fillRect(x, y, w, h, color);
  }
}

void load_png(const char *url, double scale = 1.0)
{
  HTTPClient http;

  http.begin(url);

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    M5.Lcd.printf("HTTP ERROR: %d\n", httpCode);
    http.end();
    return ;
  }

  WiFiClient *stream = http.getStreamPtr();

  pngle_t *pngle = pngle_new();
  pngle_set_draw_callback(pngle, pngle_on_draw);
  g_scale = scale; // XXX:

  uint8_t buf[2048];
  int remain = 0;
  while (http.connected()) {
    size_t size = stream->available();
    if (!size) { delay(1); continue; }

    if (size > sizeof(buf) - remain) {
      size = sizeof(buf) - remain;
    }

    int len = stream->readBytes(buf + remain, size);
    if (len > 0) {
      int fed = pngle_feed(pngle, buf, remain + len);
      if (fed < 0) {
        cls();
        M5.Lcd.printf("ERROR: %s\n", pngle_error(pngle));
        break;
      }

      remain = remain + len - fed;
      if (remain > 0) memmove(buf, buf + fed, remain);
    } else {
      delay(1);
    }
  }

  pngle_destroy(pngle);

  http.end();
}
// ===================================================

void setup()
{
  M5.begin();

  M5.Lcd.printf("Welcome.\n");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  M5.Lcd.printf("WiFi connected.\n");
}

void loop()
{
  M5.update();

  if (M5.BtnA.wasReleased()) {
    load_png("https://raw.githubusercontent.com/kikuchan/pngle/master/tests/pngsuite/PngSuite.png");
  } else if (M5.BtnB.wasReleased()) {
    load_png("https://raw.githubusercontent.com/kikuchan/pngle/master/tests/pngsuite/tbrn2c08.png", 7);
  } else if (M5.BtnC.wasReleased()) {
    load_png("https://avatars3.githubusercontent.com/u/17420673?s=240");
  }
}
