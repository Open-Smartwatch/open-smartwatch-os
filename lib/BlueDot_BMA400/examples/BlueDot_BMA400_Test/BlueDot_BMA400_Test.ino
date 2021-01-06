  
/***************************************************************************
  Example for BMA400 Sensor
  written by Thiago Barros for BlueDot UG (haftungsbeschränkt)
  BSD License

  This sketch was written for the Bosch Sensor BMA400.
  For more technical information on the BM400, please go to ------> http://www.bluedot.space

 ***************************************************************************/


#include <Wire.h>
#include "BlueDot_BMA400.h"
BlueDot_BMA400 bma400 = BlueDot_BMA400();


void setup() {
  Serial.begin(115200);

  //*********************************************************************
  //*************BASIC SETUP - READ BEFORE GOING ON!*********************
    
  //Set the I2C address of your breakout board  
  //Or ignore this, if you're using SPI Communication

  //0x14:       Alternative I2C Address (SDO pin connected to GND)
  //0x15:       Default I2C Address (SDO pin unconnected)
  
     bma400.parameter.I2CAddress = 0x15;                  //Choose I2C Address

     
  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Now choose on which power mode your device will run
  //On doubt, just leave on normal mode, that's the default value

  //0x00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0x01:     low power mode 
  //0x02:     normal mode
  
    bma400.parameter.powerMode = 0x02;                   //Choose power mode


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //We now choose the measurement range.
  //It can be set between 2g and 16g. 

  //0x00:     2g
  //0x01:     4g 
  //0x02:     8g
  //0x03:     16g
  
    bma400.parameter.measurementRange = 0x00;             //Choose measurement range


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //We now choose the measurement range.
  //It can be set between 2g and 16g. 

  //0x05:     12.5 Hz
  //0x06:     25 Hz
  //0x07:     50 Hz
  //0x08:     100 Hz
  //0x09:     200 Hz
  //0x0A:     400 Hz
  //0x0B:     800 Hz
  
    bma400.parameter.outputDataRate = 0x05;             //Choose measurement range
    

  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Finally we set the oversampling rate.
  //The oversampling rate can be set between 0 (lowest) to 3 (highest)

  //0x00:     lowest oversampling rate, lowest power consumption, lowest accuracy
  //0x01:     
  //0x02:     
  //0x03:     highest oversampling rate, highest power consumption, highest accuracy

  
    bma400.parameter.oversamplingRate = 0x03;             //Choose measurement range

    
  //*********************************************************************
  //*************ADVANCED SETUP IS OVER - LET'S CHECK THE CHIP ID!*******

  Serial.print(F("Communication with BMA400:\t"));
  if (bma400.init()==0x90)
  {
    Serial.println("Successful");   
  }
  
  else
  {
    Serial.println("Failed");
  }
  delay(1000);
  
  
  //*********************************************************************  
  //This piece of code will display during start up the chosen Power Mode 
  //There is no need to change anything here
  
  Serial.print(F("Reading Power Mode:\t\t"));
  uint8_t powerMode;
  powerMode = bma400.readPowerMode();
  
  if (powerMode == 0)
  Serial.println(F("Sleep Mode"));

  if (powerMode == 1)
  Serial.println(F("Low Power Mode"));
  
  if (powerMode == 2)
  Serial.println(F("Normal Power Mode"));
  delay(1000);
  
  
  //*********************************************************************
  //This piece of code will display during start up the chosen Measurement Range 
  //There is no need to change anything here
  
  Serial.print(F("Reading Measurement Range:\t"));
  uint8_t measurementRange;
  measurementRange = bma400.readMeasurementRange();
  
  if (measurementRange == 0)
  Serial.println(F("2g"));

  if (measurementRange == 1)
  Serial.println(F("4g"));
  
  if (measurementRange == 2)
  Serial.println(F("8g")); 

  if (measurementRange == 3)
  Serial.println(F("16g")); 
  delay(1000);
  
  
  //*********************************************************************
  //This piece of code will display during start up the chosen Output Rate 
  //There is no need to change anything here
  
  Serial.print(F("Reading Output Data Rate:\t"));
  uint8_t outputDataRate;
  outputDataRate = bma400.readOutputDataRate();

  if (outputDataRate == 5)
  Serial.println(F("12.5Hz"));

  else if (outputDataRate == 6)
  Serial.println(F("25Hz"));
  
  else if (outputDataRate == 7)
  Serial.println(F("50Hz")); 

  else if (outputDataRate == 8)
  Serial.println(F("100Hz")); 

  else if (outputDataRate == 9)
  Serial.println(F("200Hz")); 

  else if (outputDataRate == 10)
  Serial.println(F("400Hz")); 

  else if (outputDataRate == 11)
  Serial.println(F("800Hz")); 

  else
  Serial.println(outputDataRate);
  
  delay(1000);

  
  //*********************************************************************
  //This piece of code will display during start up the chosen Oversampling Rate
  //There is no need to change anything here
  
  Serial.print(F("Reading Oversampling Range:\t"));
  uint8_t oversamplingRate;
  oversamplingRate = bma400.readOversamplingRate();
  
  if (oversamplingRate == 0)
  Serial.println(F("OSR 0"));

  if (oversamplingRate == 1)
  Serial.println(F("OSR 1"));
  
  if (oversamplingRate == 2)
  Serial.println(F("OSR 2")); 

  if (oversamplingRate == 3)
  Serial.println(F("OSR 3")); 
  delay(1000);
  
  Serial.println();
}
  //*********************************************************************
  //*************NOW LET'S START MEASURING*******************************
void loop() 
{ 
  bma400.readData();
  //At this point, all measurements are finished
  //The three functions below will display the data
  //The unit of the output data is mg or 0.001 g, with g meaning g-force (with 1 g equal to about 9.8 ms^-2)
  //So with the sensor at rest on a plane surface, the measured g-force from the z-axis should be around 1000 mg
  //Non-zero values from the x and y-axes would mean that the sensor is not perfeclty perpendicular to the direction of the gravitational force 
  
  Serial.print(bma400.parameter.acc_x);
  Serial.print("\t");
  Serial.print(bma400.parameter.acc_y);
  Serial.print("\t");
  Serial.print(bma400.parameter.acc_z);
  Serial.println();
  delay(10);
  
  
 
}
