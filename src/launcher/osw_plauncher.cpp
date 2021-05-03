#include <launcher/osw_plauncher.h>
#include <faces/osw_basicface.h>

#define DEFAULTLAUNCHER_SWITCH_TIMEOUT 500
#define DEFAULTLAUNCHER_SLEEP_TIMEOUT 1500



void OswPLauncher::setup(OswHal* hal) {

  hal->getWiFi()->setDebugStream(&Serial);
  
  registerFace(new OswBasicFace());


  /*Serial.println("Loading Watchface");
  registerApp(new OswAppWatchface());
  Serial.println("Loading Stopwatch");
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
  static long lastFlush = 0;

  if (hal->btn2Down()) {
    Serial.println("Switching face");
    faces[faceIndex]->face->stop();
    faceIndex++;
    if(faceIndex == faceCount){
      faceIndex = 0;
    }
    faces[faceIndex]->face->setup((OswLauncher*)this);
    hal->clearBtn2();
  }else{
    faces[faceIndex]->face->loop();

    //Going to deepsleep on watchface 0 and Timeout
    if (faceIndex == 0 && (millis() - appOnScreenSince) > 5000) {
      hal->gfx()->fill(rgb565(0, 0, 0));
      hal->flushCanvas();
      hal->deepSleep();
    }
  }

  // limit to 30 fps and handle display flushing
  if (millis() - lastFlush > 1000 / 30 && hal->isRequestFlush()) {
    hal->flushCanvas();
    lastFlush = millis();
  }
}


void OswPLauncher::requestFlush(){
  hal->requestFlush();
}

Graphics2D* OswPLauncher::getGfx2g(){
  return hal->getCanvas()->getGraphics2D();
}

OswHal* OswPLauncher::getHal(){
  return hal;
}

void OswPLauncher::registerFace(OswFace* newFace){
  faceCount++;
  if(faceCount > 1){
    FaceRegistration *f = *faces;
    *faces = new FaceRegistration[faceCount];

    for(uint8_t i = 0 ; i < faceCount -1; i++){
      faces[i] = &f[i];
    }
    delete(f);
  }else{
    *faces = new FaceRegistration[1];  
  }
    
  FaceRegistration* face = new FaceRegistration();
  face->face = newFace;
  face->isDefault = faceCount == 1;
  strcpy(face->name,newFace->name);
  faces[faceCount - 1] = face;
}


void OswPLauncher::registerApp(OswApp* newApp){
  appCount++;

  if(appCount > 1){
    AppRegistration *a = *apps;
    *apps = new AppRegistration[appCount];

    for(uint8_t i = 0 ; i < appCount -1; i++){
      apps[i] = &a[i];
    }
    delete(a);
  }else{
    *apps = new AppRegistration[1];  
  }

  AppRegistration* app = new AppRegistration();
  app->app = newApp;
  strcpy(app->name,newApp->name);
  apps[appCount - 1] = app;
}

void OswPLauncher::stop() {
  for(int i = 0 ; i < appCount ; i++){
    apps[i]->app->stop(hal);
    delete(apps[i]);
  }
  for(int i = 0 ; i < faceCount ; i++){
    faces[i]->face->stop();
    delete(faces[i]);
  }
  delete(faces);
  delete(apps);
}


