#ifndef OSW_APP_APPSWITCHER_H
#define OSW_APP_APPSWITCHER_H

#include <osw_app.h>
#include <osw_hal.h>
#include <osw_ui.h>

#include <vector>

/**
 * @brief You can use a short press or a long press to switch between the registered applications.
 * 
 */
enum OswAppSwitcherType { SHORT_PRESS, LONG_PRESS };

/**
 * @brief App Switcher to group applications.
 *
 * The app switcher can register many applications.
 * Each of these applications is displayed in turn.
 * To switch between applications a specific button and press type could be defined.
 * 
 * The switcher itself is an application than witch can be inserted in an other switcher.
 * 
 * A pagination widget can be displayed to figure the rotation between applications.
 *
 */
class OswAppSwitcher : public OswApp {
 public:
  /**
   * @brief Construct a new Osw App Switcher object
   * 
   * @param btn Index of the button used to switch between app registered ?
   * @param type How must be pressed the button (short/long) to switch between app registered ?
   * @param enableAutoSleep Is the auto sleep enabled?
   * @param enableSleep Allow to sleep manually ?
   * @param rtcAppIndex Index of the application to disable in first.
   */
  OswAppSwitcher(Button btn, OswAppSwitcherType type, bool enableAutoSleep, bool enableSleep, uint16_t* rtcAppIndex) {
    _btn = btn;
    _type = type;
    _enableAutoSleep = enableAutoSleep;
    _enableSleep = enableSleep;
    _rtcAppIndex = rtcAppIndex;
  }

  OswAppSwitcher(){};
  
  /**
   * @brief Initialisation of the switcher and setup of the current application of the switcher.
   * 
   * @param hal 
   */
  void setup(OswHal* hal);


  void loop(OswHal* hal);
  
  
  void stop(OswHal* hal);

  /**
   * @brief Disable the pagination of the switcher.
   * 
   * Pagination allow user to switch between app registered. Button index 
   * and type of press used to switch are defined in the constructor.
   */
  void paginationDisable();

  /**
   * @brief Enable the pagination of the switcher.
   */
  void paginationEnable();

  /**
   * @brief Register a new application in the switcher
   * 
   * @param app 
   */
  void registerApp(OswApp* app);

  ~OswAppSwitcher(){};

 private:
  void cycleApp(OswHal* hal);

  /**
   * @brief Put the watch to sleep
   *
   * @param hal
   * @param fromButton Do we ask to sleep from a button.
   */
  void sleep(OswHal* hal, boolean fromButton);

  /**
   * @brief Button used to switch between app registered
   * 
   * By default, the button 1 is used.
   */
  Button _btn = BUTTON_1;

  /**
   * @brief Which kind of press used to switch between app registered ? Short or long press ?
   */
  OswAppSwitcherType _type = LONG_PRESS;

  std::vector<OswApp*> _apps;

  /**
   * @brief Index of the current displayed application.
   */
  uint16_t* _rtcAppIndex;

  /**
   * @brief Number of application registered in the switcher.
   */
  uint16_t _appCount = 0;

  bool _pagination = true;
  bool _paginationIndicator = false;
  
  /**
   * @brief Does we enable the automatic sleeping ?
   */
  bool _enableAutoSleep = false;


  bool _checked = false;

  /** 
   * @brief Does we allow manual sleeping ?
   */
  bool _enableSleep;

  bool _doSleep = false;
  bool _doSwitch = false;

  /**
   * @brief Time in milliseconds since the app is on screen.
   */
  long appOnScreenSince = 0;
};

#endif
