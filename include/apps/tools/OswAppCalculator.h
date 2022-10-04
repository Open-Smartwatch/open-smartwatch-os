#pragma once

#include <osw_hal.h>
#include <osw_ui.h>
#include "osw_app.h"


class OswAppCalculator : public OswApp {
  public:
    OswAppCalculator(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override; //checks the cursor position (iNum) and starts the appropriate function, handles button input and the cursor (position)
    virtual void stop() override;
    ~OswAppCalculator() {};
  private:
    OswUI* ui;
    void draw(int iNum, char signOfNum1, String mathOperation, char signOfNum2, String resultToString); //draws the ui
    char setSign(int8_t position, uint8_t iNum, char signOfNum1, char signOfNum2); //gets the input that tells if the number is positive or negative
    void setNum(int8_t position, uint8_t iNum); //is used to fill the array that will become the number
    String setOperator(int8_t position); //gets the input to set the operator
    String calculate(int iNum, char signOfNum1, String mathOperation, char signOfNum2); //calculates the output
};