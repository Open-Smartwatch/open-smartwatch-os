#include "./launcher/osw_plauncher.h"

#include <osw_app.h>
#include <osw_hal.h>

//apps
#include "./apps/_experiments/menu.h"
#include "./apps/main/stopwatch.h"
#include "./apps/main/watchface.h"
#include "./apps/tools/time_from_web.h"
//#include "./overlays/overlays.h"
#include "./apps/_experiments/hello_world.h"


#define DEFAULTLAUNCHER_SWITCH_TIMEOUT 500
#define DEFAULTLAUNCHER_SLEEP_TIMEOUT 1500



void OswPLauncher::setup(OswHal* hal) {

  hal->getWiFi()->setDebugStream(&Serial);
  

  //Version with struct
  appCount = 4;
  *registrations = new PLauncherAppRegistration[appCount];

  Serial.println("Loading Watchface");
  PLauncherAppRegistration* watchface = new PLauncherAppRegistration();
  watchface->app = new OswAppWatchface();
  watchface->isPinned = true;
  strcpy(watchface->name,"Watchface");
  registrations[0] = watchface;
  
  Serial.println("Loading Stopwatch");
  PLauncherAppRegistration* stopwatch = new PLauncherAppRegistration();
  stopwatch->app = new OswAppStopWatch();
  stopwatch->isPinned = true;
  strcpy(stopwatch->name,"Stopwatch");
  registrations[1] = stopwatch;
  
  Serial.println("Loading WebTime");
  PLauncherAppRegistration* webtime = new PLauncherAppRegistration();
  webtime->app = new OswAppTimeFromWeb();
  webtime->isPinned = false;
  strcpy(webtime->name,"WebTime");
  registrations[2] = webtime;
  
  Serial.println("Loading Settings");
  PLauncherAppRegistration* settings = new PLauncherAppRegistration();
  settings->app = new OswAppMenu();
  settings->isPinned = true;
  strcpy(settings->name,"Settings");
  registrations[3] = settings;
  Serial.println("Done loading");

  

  pinnedCount = 0;
  for(int i = 0 ; i < appCount ; i++){
    if(registrations[i]->isPinned){
      pinnedCount++;
    }
  }
 
  pinned = new uint8_t[pinnedCount];
  uint8_t t = 0;
  for(uint8_t i = 0 ; i < appCount ; i++){
    if(registrations[i]->isPinned){
      pinned[t] = i;
      t++;
    }
  }
   
  appIndex = pinned[0];
  //appIndex = 0;
  registrations[appIndex]->app->setup(hal);
  Serial.println("Run struct");

}

void OswPLauncher::loop(OswHal* hal) {
  
  static unsigned long appOnScreenSince = millis();
  static long lastFlush = 0;

  if (hal->btn2Down()) {
    Serial.print("Switching App to: ");
    registrations[appIndex]->app->stop(hal);
    appIndex++;
    if(appIndex == appCount){
      appIndex = 0;
    }
    Serial.println(appIndex);
    registrations[appIndex]->app->setup(hal);
    hal->clearBtn2();
  }else{
    registrations[appIndex]->app->loop(hal);
    if (appIndex == 0 && (millis() - appOnScreenSince) > 5000) {
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

void OswPLauncher::stop(OswHal* hal) {
  for(int i = 0 ; i < appCount ; i++){
    registrations[i]->app->stop(hal);
    delete(registrations[i]);
  }
  delete(pinned);
  delete(registrations);
}