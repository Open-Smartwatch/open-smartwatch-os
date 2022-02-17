#include "../include/String.h"
#include "../include/Defines.h"

bool convertToJson(const String& t, ArduinoJson::JsonVariant variant) {
  FAKE_ARDUINO_THIS_IS_NOT_IMPLEMENTED
  return variant.set(t.c_str());
}