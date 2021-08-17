#ifndef OSW_UI_H
#define OSW_UI_H

#include <osw_hal.h>

/**
 * @class OswUi
 * @brief Class to manage the user interface.
 *
 * This class can help you to manage the UI preferences like foreground color.
 *
 */
class OswUI {
 public:
  OswUI();
  void setup(OswHal* hal);
  static OswUI* getInstance();

  /**
   * @brief Get the background color to use from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getBackgroundColor(void);

  /**
   * @brief Get the second/dimmed background color to use from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getBackgroundDimmedColor(void);

  /**
   * @brief Get the foreground color to use from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getForegroundColor(void);

  /**
   * @brief Get the second/dimmed foreground color to use from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getForegroundDimmedColor(void);

  /**
   * @brief Get the primary color of the theme from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getPrimaryColor(void);

  /**
   * @brief Get the Info Color of the theme from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getInfoColor(void);

  /**
   * @brief Get the success color of the theme  from the config file.
   *
   * @return uint16_t Color code
   */
  uint16_t getSuccessColor(void);

  /**
   * @brief Get the Warning Color of the theme from the config file.
   *
   * @return uint16_t
   */
  uint16_t getWarningColor(void);

  /**
   * @brief Get the Danger Color of the theme from the config file.
   *
   * @return uint16_t
   */
  uint16_t getDangerColor(void);

  /**
   * @brief Reset the background/foreground color of the text according to the config file.
   *
   */
  void resetTextColors(void);

  /**
   * @brief Set the text cursor to be near the button selected.
   *
   * @param btn button index where we want to set the text cursor(1-4)
   */
  void setTextCursor(Button btn);

 private:
  static OswUI instance;
  OswHal* _hal;
};

#endif
