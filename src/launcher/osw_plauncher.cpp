#include <launcher/osw_plauncher.h>
#include "./overlays/overlays.h"

#include <faces/osw_basicface.h>
#include <faces/osw_analogface.h>
#include <faces/stopwatch.h>

#include "apps/tools/time_from_web.h"
#include "apps/tools/water_level.h"

#define DEFAULTLAUNCHER_SWITCH_TIMEOUT 500
#define DEFAULTLAUNCHER_SLEEP_TIMEOUT 1500
#define DEFAULTLAUNCHER_LONG_PRESS 1000



void OswPLauncher::setup(OswHal* hal) {
  faceCount = 0;
  appCount = 0;
  renderMode = Face;

  for(uint8_t i = 0  ; i < 3 ; i++){
    btnLastState[i] = false;
    btnIsDown[i] = false;
    btnGoneUp[i] = false;
  }

  _hal = hal;
  _hal->getWiFi()->setDebugStream(&Serial);
  Serial.println("Loading OswBasicFace");
  registerFace(new OswBasicFace());
  Serial.println("Loading OswAnalogFace");
  registerFace(new OswAnalogFace());
  Serial.println("Loading OswStopwatchFace");
  registerFace(new OswStopwatchFace());

  Serial.println("Loading OswAppTimeFromWeb");
  registerApp(new OswAppTimeFromWeb());
  Serial.println("Loading OswAppWaterLevel");
  registerApp(new OswAppWaterLevel());


  Serial.println("Done loading");


  if(faceCount > 0){
    faces[0]->face->setup((OswLauncher*)this);
  }else{
    Serial.println("No registered Faces - I am sad and will probably crash alone in the dark");
  }
  
}

void OswPLauncher::loop() {

  btnIsDown[0] = !_hal->btn1CurrentState();
  btnIsDown[1] = !_hal->btn2CurrentState();
  btnIsDown[2] = !_hal->btn3CurrentState();

  for(uint8_t i = 0  ; i < 3 ; i++){
    btnGoneUp[i] = btnLastState[i] == true && btnIsDown[i] == false;
    btnGoneDown[i] = btnLastState[i] == false && btnIsDown[i] == true;
    
    if(btnGoneDown[i]){
      btnIsDownSince[i] = millis();  
    }
    btnLongPress[i] = btnIsDownSince[i] < millis() - DEFAULTLAUNCHER_LONG_PRESS && btnIsDown[i] == true;
    btnLastState[i] = btnIsDown[i];

    if(btnSuppressUntilUpAgain[i] == true && btnIsDown[i]){
      btnGoneUp[i] = false;
      btnGoneDown[i] = false;
      btnIsDownSince[i] = millis();  
      btnLongPress[i] = false;
      btnIsDown[i] = false;
    }else{
      btnSuppressUntilUpAgain[i] = false;
    }
  }



  switch(renderMode){
    case Face:
      handleFace();
    break;
    case Menu:
      handleMenu();
    break;
    case App:
      handleApp();
    break;
  }
  
  if(btnLongPress[0] == true && btnLongPress[1] == true){
    backToFaceMode();
  }

  if(btnIsDown[0] == true && btnIsDown[2] == true){
    uint8_t brightness = _hal->btn1Down() - 255;
    _hal->gfx()->fillFrame(120 - 4, 230 - 4, 8, 8, rgb565(brightness, brightness, brightness));
  }

}

void OswPLauncher::resetButtons(){
for(uint8_t i = 0  ; i < 3 ; i++){
    btnSuppressUntilUpAgain[i] = true;
  }
}

void OswPLauncher::backToFaceMode(){
  switch(renderMode){
    case Face:
      faces[faceIndex]->face->stop();
      faces[faceIndex]->face->setup((OswLauncher*)this);
    break;
    case Menu:
      faces[faceIndex]->face->setup((OswLauncher*)this);
    break;
    case App:
      apps[appIndex]->app->stop(_hal);
      faces[faceIndex]->face->setup((OswLauncher*)this);
    break;
  }
  renderMode = Face;
  resetButtons();
}

void OswPLauncher::handleFace(){

  
  if(btnIsDown[0] == true && btnIsDown[2] == true && renderMode == Face){
    Serial.println("Switch to MenuMode");
    renderMode = Menu;
    resetButtons();
  }

  static unsigned long faceOnScreenSince = millis();
  //face up
  if (btnGoneUp[2] == true) {
    faces[faceIndex]->face->stop();
    faceIndex++;
    if(faceIndex == faceCount){
      faceIndex = 0;
    }
    faces[faceIndex]->face->setup((OswLauncher*)this);
    faceOnScreenSince = millis();
  }

  //face down
  if (btnGoneUp[0] == true) {
    faces[faceIndex]->face->stop();
    if(faceIndex == 0){
      faceIndex = faceCount;
    }
    faceIndex--;
    faces[faceIndex]->face->setup((OswLauncher*)this);
    faceOnScreenSince = millis();
  }
  
  //Trigger first action simple button press
  if (btnGoneDown[1] == true) {
    faces[faceIndex]->face->action();
  }
  //Trigger first action simple button press
  if (btnLongPress[1] == true) {
    faces[faceIndex]->face->action2();
  }

  faces[faceIndex]->face->loop();

  //Going to deepsleep on watchface 0 and Timeout
  if (faceIndex == 0 && (millis() - faceOnScreenSince) > 5000) {
    _hal->gfx()->fill(rgb565(0, 0, 0));
    _hal->flushCanvas();
    _hal->deepSleep();
  }
}

void OswPLauncher::handleMenu(){

  //menu up
  if (btnGoneUp[2] == true) {
    menuIndex++;
    if(menuIndex == appCount){
      menuIndex = 0;
    }
  }

  //menu down
  if (btnGoneUp[0] == true) {
    if(menuIndex == 0){
      menuIndex = appCount;
    }
    menuIndex--;
  }
  
  //menu down
  if (btnGoneUp[1] == true) {
    renderMode = App;
    appIndex = menuIndex;
    apps[appIndex]->app->setup(_hal);
  }

  printMenuEntry(menuIndex);
}


void OswPLauncher::handleApp(){
  apps[appIndex]->app->loop(_hal);
}


void OswPLauncher::printMenuEntry(int16_t index) {
    
    uint8_t sWidth = _hal->getCanvas()->width();
    uint8_t sHeight = _hal->getCanvas()->height();
    
    uint8_t cx = sWidth / 2;
    uint8_t cy = sHeight / 2;
    uint8_t  hy,ly;
    uint8_t  hx,lx;

    int16_t  x1, y1;
    uint16_t  w, h;

    uint8_t barPadding = 5;
    uint16_t backgroundColor = 0x5B0D;
    uint16_t textColor = 0xCE9B;
    uint16_t barColor = 0xFC68;
    uint16_t barSelectionColor = 0x7DEC;
    uint16_t barTextcolor = 0xFFBF;


    _hal->getCanvas()->fillRect(0, 0, 240, 240, backgroundColor);
    _hal->getCanvas()->setTextColor(rgb565(200, 255, 200));

    //Center
    _hal->getCanvas()->setTextSize(3);
    _hal->getCanvas()->getTextBounds(apps[index]->name, 0, 0, &x1, &y1, &w, &h);  
    _hal->getCanvas()->setCursor(cx - (w / 2) , cy - (h / 2));
    //_hal->getCanvas()->fillRect(0, sHeight / 2 - h / 2 - barPadding, sWidth , barPadding * 2 + h , selectionInProgress ? barSelectionColor : barColor);

    _hal->getCanvas()->setTextColor(barTextcolor);
    _hal->getCanvas()->print(apps[index]->name);

    _hal->getCanvas()->setTextColor(textColor);
    
    //First Level
    hx = cx + sWidth / 8;
    hy = cy - h * 1.5f;
    lx = cx - sWidth / 8;
    ly = cy + h * 1.5f;

    if(index - 1 >= 0){
        _hal->getCanvas()->setTextSize(2);
        _hal->getCanvas()->getTextBounds(apps[index - 1]->name, 0, 0, &x1, &y1, &w, &h);  

        _hal->getCanvas()->setCursor(hx - (w / 2) , hy - (h / 2));
        _hal->getCanvas()->print(apps[index - 1]->name);
    }

    if(index + 1 < appCount){
        _hal->getCanvas()->setTextSize(2);
        _hal->getCanvas()->getTextBounds(apps[index + 1]->name, 0, 0, &x1, &y1, &w, &h);  

        _hal->getCanvas()->setCursor(lx - (w / 2) , ly - (h / 2));
        _hal->getCanvas()->print(apps[index + 1]->name);
    }

    //Second Level
    hx = hx + sWidth / 7;
    hy = hy - h * 1.5f;
    lx = lx - sWidth / 7;
    if(lx < 0 ){
        lx = 0;
    }
    ly = ly + h * 1.5f;

    if(index - 2 >= 0){
        _hal->getCanvas()->setTextSize(1);
        _hal->getCanvas()->getTextBounds(apps[index - 2]->name, 0, 0, &x1, &y1, &w, &h);  
        _hal->getCanvas()->setCursor(hx - (w / 2) , hy - (h / 2));
        _hal->getCanvas()->print(apps[index - 2]->name);
    }
    if(index + 2 < appCount){
        _hal->getCanvas()->setTextSize(1);
        _hal->getCanvas()->getTextBounds(apps[index + 2]->name, 0, 0, &x1, &y1, &w, &h);  
        _hal->getCanvas()->setCursor(lx - (w / 2) , ly - (h / 2));
        _hal->getCanvas()->print(apps[index + 2]->name);
    }

  requestFlush();
}

void OswPLauncher::requestFlush(){
   drawOverlays(_hal);
  _hal->requestFlush();
}

Graphics2D* OswPLauncher::getGfx2g(){
  return _hal->getCanvas()->getGraphics2D();
}

OswHal* OswPLauncher::getHal(){
  return _hal;
}

void OswPLauncher::registerFace(OswFace* newFace){
  faceCount++;
  FaceRegistration* face = new FaceRegistration();
  (*face).face = newFace;
  face->isDefault = faceCount == 1;
  strcpy(face->name,newFace->name);
  faces.push_back(face);
}


void OswPLauncher::registerApp(OswApp* newApp){
  appCount++;
  AppRegistration* app = new AppRegistration();
  (*app).app = newApp;
  strcpy(app->name,newApp->getName());
  apps.push_back(app);
}

void OswPLauncher::stop() {
  for(uint8_t i = 0 ; i < appCount ; i++){
    apps[i]->app->stop(_hal);
    delete(apps[i]);
  }
  for(uint8_t i = 0 ; i < faceCount ; i++){
    faces[i]->face->stop();
    delete(faces[i]);
  }
}


