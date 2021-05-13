
#include "./apps/tools/metronome.h"
#include <gfx_util.h>
#include <Tone32.h> //this project uses Tone32 library https://github.com/lbernstone/Tone32 for generating tick sound
#include <osw_app.h>
#include <osw_hal.h>
#include "config.h"

uint16_t bpm = 120; //Default BPM

//Define buzzer and vibration motor pin here
//please set those GPIOs as output and set their initial value to LOW in main.cpp setup
#define BUZZER_PIN 2
#define VIBRATION_PIN 14
#define BUZZER_CHANNEL 0

int period;
unsigned long time_now = 0;
uint16_t pos = 60;
bool run = true;
uint16_t beat = 4;
uint16_t vibrate = 25;
int TONE = 4699;

void OswAppMetronome::setup(OswHal* hal) {
}

void OswAppMetronome::loop(OswHal* hal) {
 //Button logic
  if (hal->btnHasGoneDown(BUTTON_1) && run == true)
  {
    run = false;
  }

  else if (hal->btnHasGoneDown(BUTTON_1) && run == false)
  {
    run = true;
  }

  if (run == true && bpm < 300){
    if (hal->btnHasGoneDown(BUTTON_3)) {
      bpm ++;
    }

    if (hal->btnHasGoneDown(BUTTON_2) && bpm > 40)
    {
      bpm --;
    }  
  }

  else
  {
    if (hal->btnHasGoneDown(BUTTON_3) && beat < 4) 
    {
      beat ++;
    }

    if (hal->btnHasGoneDown(BUTTON_2) && beat >2) {
      beat --;
    }
  
  }
  
  //Tick Timing
  period = (60000/bpm);   

  if(millis() > time_now + period && run == true)
  {
    time_now = millis();
        
    if (pos >= 180 && beat == 4)
    {
      pos = 60;
      vibrate = 40;
      TONE = 5274;
    }

    else if (pos >= 140 && beat == 3)
    {
     pos = 60;
     vibrate = 40;
     TONE = 5274;
    }

    else if (pos >= 100 && beat == 2)
    {
      pos = 60;
      vibrate = 40;
      TONE = 5274;
    }

    else
    {
      pos = pos + 40;
      vibrate = 25;
      TONE = 4699;
    }

    digitalWrite (VIBRATION_PIN, HIGH);
    tone(BUZZER_PIN, TONE, 10, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay (vibrate);
    digitalWrite (VIBRATION_PIN, LOW);
  }

  //LCD graphic stuff
  //I have defined colors in config.h If you don't want to have colors defined globally, please replace BACKROUND, PRIMARY etc. with colors of your choice
  hal->gfx()->fill(BACKGROUND); 
  if (pos == 60)
  {
    hal->gfx()->fillCircle(pos, 134, 10, ORANGE);
  }

  else
  {
   hal->gfx()->fillCircle(pos, 134, 10, ACCENT);
  }

  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextColor(PRIMARY, BACKGROUND); 
  hal->gfx()->setTextLeftAligned();
  hal->gfx()->setTextCursor(53, 90); 
  hal->gfx()->print(beat); 
  hal->gfx()->print("/4  ");          
  hal->gfx()->print(bpm);
  hal->gfx()->print(" bpm");

  hal->gfx()->drawCircle(60, 134, 14, PRIMARY);
  hal->gfx()->drawCircle(100, 134, 14, PRIMARY);
  hal->gfx()->drawCircle(140, 134, 14, PRIMARY);
  hal->gfx()->drawCircle(180, 134, 14, PRIMARY);


  //+button
    hal->gfx()->fillCircle(200, 57, 10, SECONDARY);
    hal->gfx()->drawVLine(200, 53, 9, BACKGROUND);
    hal->gfx()->drawHLine(196, 57, 9, BACKGROUND);
  //-button
    hal->gfx()->fillCircle(200, 183, 10, SECONDARY);
    hal->gfx()->drawHLine(196, 183, 9, BACKGROUND);
  //pause/start button
    hal->gfx()->fillCircle(40, 183, 10, SECONDARY);
    if (run == true)
    {
      hal->gfx()->drawVLine(37, 179, 9, BACKGROUND);
      hal->gfx()->drawVLine(43, 179, 9, BACKGROUND);
    }

    else
    {
      hal->gfx()->drawTriangle(37, 179, 37, 187, 43, 183, BACKGROUND);
    }
    
  hal->requestFlush();
}


void OswAppMetronome::stop(OswHal* hal) {
  }
