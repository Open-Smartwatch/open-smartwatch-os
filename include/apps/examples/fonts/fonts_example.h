#ifndef OSW_APP_FONTS_EXAMPLE_H
#define OSW_APP_FONTS_EXAMPLE_H

#include <osw_app.h>
class OswAppFontsExample: public OswApp {
 public:
  OswAppFontsExample(void){};
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppFontsExample(){};
 private:
};

#endif
