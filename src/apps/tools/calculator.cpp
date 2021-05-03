#include "./apps/tools/calculator.h"

#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <time.h>
#include <map>

#include "osw_ui_util.h"

#include "bma400_defs.h"

#define COLOR_BLACK rgb565(0, 0, 0)

void OswAppCalculator::setup(OswHal* hal) {

}

void OswAppCalculator::loop(OswHal* hal) {
  //possibly the easiest way to do this would be to have button 2 handle going through numbers 0-9 and button 3 to cycle through operators like +, -, /, * etc
  //basically just write this to some buffer until the user cycles to the = and then do the math and override the on screen buffer with the calculated value
  //it's possible that this may beed each button to do double-duty. if the user hits btn2, they are going to cycle through digits. they can finalize their input by hitting button 3, and then hit button 3 again to begin cycling through operators
  //once they've landed on the operator they want, hit button 2 to write it to the buffer. this way, we don't have to make assumptions about intent etc
  //so maybe something like user enters a value, like 34, hits button 3, save that to a list or a map of int int and save the operators in a map of int string, and use a switch statement, working our way through the lists
  //so let's say there's 2 maps and they look like this:
  // numbers map: [0]:10,[1]:12,[2]:10
  // operators map: [0]:+, [1]:+
  //we would use the switch statement to find a case for the first value in the operators map, +, and then within that case, we would take numbersmap[0]+nubmersmap[1], and store that output somewhere. if there is another operator in the operatorsmap,
  //go through the process again. 
  std::map<int, double> valuesMap;
  std::map<int, char> operatorsMap;
  double output = 0; 


  if(hal->btn2Down()) {
    
    hal->clearBtn2();
  }
  if (hal->btn3Down()) {
    
    hal->clearBtn3();
  }

  hal->requestFlush();
  hal->getCanvas()->getGraphics2D()->fill(COLOR_BLACK);


  hal->requestFlush();
}

void OswAppCalculator::stop(OswHal* hal) {
}