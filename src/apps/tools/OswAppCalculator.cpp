/*
    a calculator for addition, subtraction, multiplication, division, square roots, powers, num1 * 10 ^ num2
    with postitive and negativ numbers + generating random numbers
    press btn2 + btn3 to move the cursor a position back
*/

#include "./apps/tools/OswAppCalculator.h"

#include "config_defaults.h"
#include "gfx_util.h"
#include <OswAppV1.h>
#include "osw_hal.h"
#include "osw_ui.h"

#include <osw_config_keys.h>

void OswAppCalculator::setup() {}

void OswAppCalculator::loop() {
    OswHal* hal = OswHal::getInstance();

    //handle button input
    if(hal->btnHasGoneDown(BUTTON_2) && hal->btnHasGoneDown(BUTTON_3)) {
        iNum--; //sets the cursor a position back, if you made a mistake
        delay(500); //prevents input while releasing buttons
        position = 0; //resetes the previous number to 0
    } else if(hal->btnHasGoneUp(BUTTON_1)) {
        iNum++;
        position = 0;
    } else if (hal->btnHasGoneUp(BUTTON_3)) {
        position++;
    } else if (hal->btnHasGoneUp(BUTTON_2)) {
        position--;
    }

    //calls the functions and controlls the cursor (position)
    if(iNum == 0) { //sets sign of num1
        signOfNum1 = setSign(position, iNum, signOfNum1, signOfNum2);
        position = (position > 1) ? 0 : position;
        position = (position < 0) ? 1 : position;
    } else if (iNum < 7) { //sets num1
        position = (position > 9) ? 0 : position;
        position = (position < 0) ? 9 : position;
        setNum(position, iNum);
    } else if(iNum == 7) { //sets the operator
        mathOperation = setOperator(position);
        position = (position < 0) ? 7 : position;
        position = (position > 8) ? 0 : position;
    } else if(iNum == 8) { //sets sign of num2
        iNum = ((mathOperation == "square root") || (mathOperation == "random")) ? 15 : 8; //skips to the end because no further input is requierd
        signOfNum2 = setSign(position, iNum, signOfNum1, signOfNum2);
        position = (position > 1) ? 0 : position;
        position = (position < 0) ? 1 : position;
    } else if (iNum > 8 && iNum < 15) { //sets num2
        position = (position > 9) ? 0 : position;
        position = (position < 0) ? 9 : position;
        setNum(position, iNum);
    } else if (iNum == 15) { //calculates the result
        resultToString = calculate(iNum, signOfNum1, mathOperation, signOfNum2);
        iNum++;
    } else if (iNum > 15) { //resets num1, num2 and the operator. The result stays
        memset(num1,0,sizeof(num1));
        memset(num2,0,sizeof(num2));
        mathOperation = "+";
        signOfNum1 = '+';
        signOfNum2 = '+';
        iNum = 0;
    }
    draw(iNum, signOfNum1, mathOperation, signOfNum2, resultToString);
}


char OswAppCalculator::setSign(int8_t position, uint8_t iNum, char signOfNum1, char signOfNum2) {

    if (position < 0) {
        position = 1;
    }
    if (position > 1) {
        position = 0;
    }

    if (position == 0) {
        if(iNum == 0) {
            signOfNum1 = '+';
        } else if(iNum == 8) {
            signOfNum1 = '+';
        }
    } else if (position == 1) {
        if(iNum == 0) {
            signOfNum1 = '-';
        } else if(iNum == 8) {
            signOfNum1 = '-';
        }
    }

    return signOfNum1;
}

void OswAppCalculator::setNum(int8_t position, uint8_t iNum) {

    if(iNum < 7) {
        num1[iNum-1] = position;
    } else if(iNum > 6) {
        num2[iNum-9] = position;
    }
}

String OswAppCalculator::setOperator(int8_t position) {
    String operation;

    if (position < 0) {
        position = 7;
    }
    if (position > 7) {
        position = 0;
    }

    switch(position) {
    case 0:
        operation = "+";
        break;
    case 1:
        operation = "-";
        break;
    case 2:
        operation = "*";
        break;
    case 3:
        operation = "/";
        break;
    case 4:
        operation = "square root";
        break;
    case 5:
        operation = "^";
        break;
    case 6:
        operation = "x10^";
        break;
    case 7:
        operation = "random";
        break;
    }

    return operation;
}

String OswAppCalculator::calculate(int iNum, char signOfNum1, String mathOperation, char signOfNum2) {

    double Num1;
    double Num2;
    double result = 0;
    Num1 = num1[0];
    for (uint8_t i=1; i<6; i++) {
        Num1 = Num1 *10 + num1[i];
    }

    Num1 = Num1 / 100;

    Num2 = num2[0];
    for (uint8_t i=1; i<6; i++) {
        Num2 = Num2 *10 + num2[i];
    }

    Num2 = Num2 / 100;

    if (signOfNum1 == '-') {
        Num1 = 0 - Num1;
    }

    if (signOfNum2 == '-') {
        Num2 = 0 - Num2;
    }

    if (mathOperation == "+") {
        result = Num1 + Num2;
    } else if (mathOperation == "-") {
        result = Num1 - Num2;
    } else if (mathOperation == "*") {
        result = Num1 * Num2;
    } else if (mathOperation == "/") {
        result = Num1 / Num2;
    } else if (mathOperation == "square root") {
        result = sqrtf(Num1);
    } else if (mathOperation == "^") {
        result = powf(Num1, Num2);
    } else if (mathOperation == "x10^") {
        result = powf(10, Num2) * Num1;
    } else if (mathOperation == "random") {
        result = random(-100000, 100001);
        result = result / 100;
    }

    String resultToString = String(result);

    return resultToString;

}

void OswAppCalculator::draw(int iNum, char signOfNum1, String mathOperation, char signOfNum2, String resultToString) {
    OswHal* hal = OswHal::getInstance();

    hal->gfx()->setTextSize(4);
    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(19, 70);

    if(iNum == 0) {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(signOfNum1);
        hal->gfx()->setTextSize(4);

    } else {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(signOfNum1);
        hal->gfx()->setTextSize(4);

    }

    if(iNum == 1) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num1[0]);

    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num1[0]);
    }

    if(iNum == 2) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num1[1]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num1[1]);
    }

    if(iNum == 3) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num1[2]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num1[2]);
    }

    if(iNum == 4) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num1[3]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num1[3]);
    }

    if(iNum == 5) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(".");
        hal->gfx()->print(num1[4]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(".");
        hal->gfx()->print(num1[4]);
    }

    if(iNum == 6) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num1[5]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num1[5]);
    }

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextCursor(125, 102);
    if(iNum == 7) {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(mathOperation);
        hal->gfx()->setTextSize(4);
    } else {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(mathOperation);
        hal->gfx()->setTextSize(4);
    }

    hal->gfx()->setTextLeftAligned();
    hal->gfx()->setTextCursor(19, 140);

    if(iNum == 8) {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(signOfNum2);
        hal->gfx()->setTextSize(4);
    } else {
        hal->gfx()->setTextSize(3);
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(signOfNum2);
        hal->gfx()->setTextSize(4);
    }

    if(iNum == 9) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num2[0]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num2[0]);
    }

    if(iNum == 10) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num2[1]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num2[1]);
    }

    if(iNum == 11) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num2[2]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num2[2]);
    }

    if(iNum == 12) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num2[3]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num2[3]);
    }

    if(iNum == 13) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(".");
        hal->gfx()->print(num2[4]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(".");
        hal->gfx()->print(num2[4]);
    }

    if(iNum == 14) {
        hal->gfx()->setTextColor(ui->getSuccessColor());
        hal->gfx()->print(num2[5]);
    } else {
        hal->gfx()->setTextColor(ui->getForegroundColor());
        hal->gfx()->print(num2[5]);
    }

    hal->gfx()->fillFrame(0, 160, 240, 10, ui->getSuccessColor());

    hal->gfx()->setTextCenterAligned();
    hal->gfx()->setTextSize(3);
    hal->gfx()->setTextCursor(120, 210);
    hal->gfx()->setTextColor(ui->getSuccessColor());
    hal->gfx()->print(resultToString);

    hal->gfx()->setTextMiddleAligned();
    hal->gfx()->setTextColor(ui->getForegroundColor());
    hal->gfx()->setTextCursor(214, 57);
    hal->gfx()->print("+");
    hal->gfx()->setTextCursor(214, 184);
    hal->gfx()->print("-");
    hal->gfx()->setTextCursor(31, 184);
    hal->gfx()->print(">");
}

void OswAppCalculator::stop() {}