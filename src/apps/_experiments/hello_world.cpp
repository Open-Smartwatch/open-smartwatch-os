
#include "./apps/_experiments/hello_world.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
//define global scope variables
bool red = false;

void OswAppHelloWorld::setup(OswHal* hal) {

  //this is where you initialise code

}

void OswAppHelloWorld::loop(OswHal* hal) {

  //This section of the code is where you can write code that will loop.

  //Here you can execute code on a button press. For this example I am changing the value of a variable.

  if (hal->btn3Down()) {
   red = true; 
  }

  if (hal->btn2Down()) {
   red = false;
  }

  //As the variable 'red' is changed, this if loop adjusts the colour of the 'hello world' text 

  if (red) {
    hal->getCanvas()->fillScreen(0);
    hal->getCanvas()->setTextSize(2);
    hal->getCanvas()->setTextColor(rgb565(255, 0, 0));
    hal->getCanvas()->setCursor(50, 120); //set (X,Y) coordinates for the 240x204px display
    hal->getCanvas()->print("Hello World");

    //set a new piece of text with different properties

    hal->getCanvas()->setCursor(160, 180);
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->print("Off");

    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
    hal->getCanvas()->setCursor(160, 60);
    hal->getCanvas()->print("Red");
   
   } else {

    hal->getCanvas()->fillScreen(0);
    hal->getCanvas()->setTextSize(2);
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255)); //colour changes
    hal->getCanvas()->setCursor(50, 120);

    hal->getCanvas()->print("Hello World ");
    hal->getCanvas()->setCursor(160, 180);
    hal->getCanvas()->print("Off");

    hal->getCanvas()->setCursor(160, 60);
    hal->getCanvas()->print("Red");
    
   } 
   hal->requestFlush(); //send updated text to display                

}

void OswAppHelloWorld::stop(OswHal* hal) {
  // this is where you de-initialize stuff
}

