#include <launcher/osw_plauncher.h>
#include <faces/osw_basicface.h>
#include <faces/osw_analogface.h>
#include <faces/stopwatch.h>
#include "./overlays/overlays.h"

#define DEFAULTLAUNCHER_SWITCH_TIMEOUT 500
#define DEFAULTLAUNCHER_SLEEP_TIMEOUT 1500



void OswPLauncher::setup(OswHal* hal) {
  faceCount = 0;
  appCount = 0;
  _hal = hal;
  _hal->getWiFi()->setDebugStream(&Serial);
  Serial.println("Loading OswBasicFace");
  registerFace(new OswBasicFace());
  Serial.println("Loading OswAnalogFace");
  registerFace(new OswAnalogFace());
  Serial.println("Loading OswStopwatchFace");
  registerFace(new OswStopwatchFace());

  /*  Serial.println("Loading Stopwatch");
  registerApp(new OswAppStopWatch());
  Serial.println("Loading WebTime");
  registerApp(new OswAppTimeFromWeb());
  Serial.println("Loading Settings");
  registerApp(new OswAppMenu());
  Serial.println("Done loading");
*/

  if(faceCount > 0){
    faces[0]->face->setup((OswLauncher*)this);
  }else{
    Serial.println("No registered Faces - I am sad and will probably crash alone in the dark");
  }
  
}

void OswPLauncher::loop() {

  static unsigned long appOnScreenSince = millis();


  //face up
  if (_hal->btn3Down()) {
    faces[faceIndex]->face->stop();
    faceIndex++;
    if(faceIndex == faceCount){
      faceIndex = 0;
    }
    faces[faceIndex]->face->setup((OswLauncher*)this);
    appOnScreenSince = millis();
    _hal->clearBtn3();
  }

  //face down
  if (_hal->btn1Down()) {
    faces[faceIndex]->face->stop();
    if(faceIndex == 0){
      faceIndex = faceCount;
    }
    faceIndex--;
    faces[faceIndex]->face->setup((OswLauncher*)this);
    appOnScreenSince = millis();
    _hal->clearBtn1();
  }
  
  //Trigger first actionon simple button press
  if (_hal->btn2Down()) {
    faces[faceIndex]->face->action();
    _hal->clearBtn2();
  }

  faces[faceIndex]->face->loop();

  //Going to deepsleep on watchface 0 and Timeout
  if (faceIndex == 0 && (millis() - appOnScreenSince) > 5000) {
    _hal->gfx()->fill(rgb565(0, 0, 0));
    _hal->flushCanvas();
    _hal->deepSleep();
  }
    //temp comment

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
  strcpy(app->name,newApp->name);
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


