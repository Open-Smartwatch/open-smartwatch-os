#ifndef OSW_APP_H
#define OSW_APP_H

#include <osw_hal.h>

/**
 * @brief Class to defined an new application in the smartwatch
 * 
 * Each watchface, game or tool in the smartwatch is is an application.
 * 
 */
class OswApp {
 public:

 /**
  * @brief The setup function is used to initialise the application.
  * 
  * The initialisation of the application come when we display the application.
  * 
  * @param hal 
  */
  virtual void setup(OswHal* hal) = 0;

  /**
   * @brief The loop function define what the application have to do.
   * 
   * This function loop until the application is on the screen.
   * 
   * @param hal 
   */
  virtual void loop(OswHal* hal) = 0;
  
  
  virtual void stop(OswHal* hal) = 0;
};

#endif