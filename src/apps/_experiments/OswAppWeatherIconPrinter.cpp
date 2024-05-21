#ifdef OSW_FEATURE_WEATHER
#include "apps/_experiments/OswAppWeatherIconPrinter.h"

OswAppWeatherIconPrinter::OswAppWeatherIconPrinter() {}


void OswAppWeatherIconPrinter::getHal(OswHal* h) {
    this->hal = h;
}

void OswAppWeatherIconPrinter::setColors(uint32_t cloudBright, uint32_t cloudDark, uint32_t thunderbolt, uint32_t sun, uint32_t droplet) {
    this->cloudBrightColor = cloudBright;
    this->cloudDarkColor = cloudDark;
    this->thunderboltColor = thunderbolt;
    this->sunColor = sun;
    this->dropletColor = droplet;
}

void OswAppWeatherIconPrinter::drawThunderBolt(int x, int y, float k) {
    x = x - (10 * k);
    y = y - (3 * k);
    int j = (3 * k) + 1;
    if (k <= 1) {
        this->hal->gfx()->drawLine(x, y, x + 3, y + 5, this->thunderboltColor);
        this->hal->gfx()->drawLine(x + 1, y, x + 4, y + 5, this->thunderboltColor);
        this->hal->gfx()->drawLine(x + 2, y, x + 5, y + 5, this->thunderboltColor);

        this->hal->gfx()->drawLine(x + 4, y + 4, x + 7, y + 8, this->thunderboltColor);
        this->hal->gfx()->drawLine(x + 5, y + 4, x + 8, y + 9, this->thunderboltColor);
        this->hal->gfx()->drawLine(x + 6, y + 4, x + 9, y + 10, this->thunderboltColor);
    } else {
        for (int i = 0; i < j; i++) {
            this->hal->gfx()->drawLine(x + i, y, (x + i) + ((3.0 / 5.0) * 5 * k), y + (5 * k), thunderboltColor);
        }
        for (int i = 0; i < j; i++) {
            this->hal->gfx()->drawLine((x + (j / 2)) + ((3.0 / 5.0) * 5 * k) + i, y + (5 * k) - 1, x + (7 * k), (y + (5 * k) - 1) + (k * 6), thunderboltColor);
        }
    }
}

void OswAppWeatherIconPrinter::drawFog(int x, int y, float k, int mistFog) {
    this->hal->gfx()->drawThickLine(x - (k * 10), y, x + (k * 10), y, 2 * k, this->cloudBrightColor);
    this->hal->gfx()->drawThickLine(x - (k * 10), y + (k * 12), x + (k * 10), y + (k * 12), 2 * k, this->cloudBrightColor);

    if (mistFog >= 1) {
        this->hal->gfx()->drawThickLine(x - (k * 10), y - (12 * k), x + (k * 10), y - (12 * k), 2 * k, this->cloudBrightColor);
    }
}

void OswAppWeatherIconPrinter::drawSun(int x, int y, float k) {
    int radius = k * 15;
    this->hal->gfx()->fillCircle(x, y, radius, this->sunColor);
}

void OswAppWeatherIconPrinter::drawMoon(int x, int y, int radius) {
    this->hal->gfx()->fillCircle(x, y, radius, this->cloudBrightColor);
}
/*draw a cloud given: x coordinate and y coordinate.
  color: rgb888 color
*/
void OswAppWeatherIconPrinter::drawCloud(int x, int y, uint32_t color, float k) {
    float radius = 15 * k;
    float k1 = 12 * k;
    float j1 = 5 * k;
    float k2 = 6 * k;
    float j2 = 1 * k;
    float k3 = 14 * k;
    this->hal->gfx()->fillCircle(x, y, radius, color);
    this->hal->gfx()->fillCircle(x - k1, y + j1, radius - j1, color);
    this->hal->gfx()->fillCircle(x - k2, y - j2, radius - j1, color);
    this->hal->gfx()->fillCircle(x + k3, y + j1, radius - j1, color);
}

void OswAppWeatherIconPrinter::drawDroplet(int x, int y, float k, uint32_t color) {
    int radius = (int)k + 1;
    this->hal->gfx()->drawThickLine(x, y, x + (5 * k), y + (5 * k), radius, color, true);
}

void OswAppWeatherIconPrinter::drawRain(int x, int y, float k) {
    this->drawDroplet(x, y, k, this->dropletColor);
    this->drawDroplet(x + (12 * k), y + (6 * k), k, this->dropletColor);
    this->drawDroplet(x + (17 * k), y, k, this->dropletColor);
}

void OswAppWeatherIconPrinter::drawSnow(int x, int y, int level, float k) {
    switch (level) {
    case 1:
        this->hal->gfx()->fillCircle(x + (10 * k), y + (3 * k), (3 * k) + 0.2, this->cloudBrightColor);
        break;
    case 2:
        this->hal->gfx()->fillCircle(x, y + (2 * k), (3 * k) + 0.2, this->cloudBrightColor);
        this->hal->gfx()->fillCircle(x + (18 * k), y + (2 * k), (3 * k) + 0.2, this->cloudBrightColor);
        break;
    case 3:
        this->hal->gfx()->fillCircle(x, y, (3 * k) + 0.2, this->cloudBrightColor);
        this->hal->gfx()->fillCircle(x + (10 * k), y + 5, (3 * k) + 0.2, this->cloudBrightColor);
        this->hal->gfx()->fillCircle(x + (18 * k), y, (3 * k) + 0.2, this->cloudBrightColor);
    default:
        break;
    }
}

void OswAppWeatherIconPrinter::drawIcon(int code, int x, int y, float k) {
    switch (code) {
    case 0: // sun
        this->drawSun(x, y, k);
        break;
    case 1: // clouds min
        this->drawSun(x - (15 * k), y - (5 * k), k);
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 2: // clouds medium
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 3: // heavy clouds
        this->drawCloud(x - (3 * k), y, this->cloudDarkColor, k);
        this->drawCloud(x + (10 * k), y, this->cloudBrightColor, k);
        break;
    case 4: // mist
        this->drawFog(x, y, k, 0);
        break;
    case 5: // fog
        this->drawFog(x, y, k, 1);
        break;
    case 6: // snow min
        this->drawSnow(x - (10 * k), y + (20 * k), 1, k);
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 7: // snow med
        this->drawSnow(x - (10 * k), y + (20 * k), 2, k);
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 8: // snow heavy
        this->drawSnow(x - (10 * k), y + (20 * k), 3, k);
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 9: // rain min
        this->drawRain(x - (5 * k), y + (20 * k), k);
        this->drawSun(x - (15 * k), y - (5 * k), k);
        this->drawCloud(x, y, this->cloudBrightColor, k);
        break;
    case 10: // rain med
        this->drawCloud(x, y, this->cloudBrightColor, k);
        this->drawRain(x - (5 * k), y + (20 * k), k);
        break;
    case 11: // rain heavy
        this->drawCloud(x - (3 * k), y, this->cloudDarkColor, k);
        this->drawCloud(x + (10 * k), y, this->cloudBrightColor, k);
        this->drawRain(x - (5 * k), y + (20 * k), k);
        break;
    case 12: // thunderstorm 1
        this->drawCloud(x, y, this->cloudBrightColor, k);
        this->drawThunderBolt(x + (5 * k), y + (20 * k), k);
        break;
    case 13: // thunderstorm 2
        this->drawThunderBolt(x + (5 * k), y + (20 * k), k);
        this->drawThunderBolt(x + (20 * k), y + (18 * k), k);
        this->drawCloud(x - (3 * k), y, this->cloudDarkColor, k);
        this->drawCloud(x + (10 * k), y, this->cloudBrightColor, k);
        break;
    case 14:
        this->hal->gfx()->setTextCursor(x, y);
        if (k <= 1) {
            this->hal->gfx()->setTextSize(1);
        } else {
            this->hal->gfx()->setTextSize(2);
        }
        this->hal->gfx()->print("!");
        break;
    case 15: // unknown
        this->hal->gfx()->setTextCursor(x, y);
        this->hal->gfx()->print("?");

    default:
        break;
    }
}
#endif
