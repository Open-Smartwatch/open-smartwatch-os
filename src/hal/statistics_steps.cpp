
#include <Preferences.h>
#include <osw_hal.h>

#define PREFS_STEPS "PS"
#define PREFS_STEPS_DAYLASTCHECKED "L"
#define PREFS_STEPS_STATS "S"
#define PREFS_STEPS_ALL "A"

void OswHal::setupSteps() {
#ifdef OSW_FEATURE_STATS_STEPS
  Preferences prefs;
  prefs.begin(PREFS_STEPS, false); // Open in RW, just in case
  if(prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) != sizeof(this->_stepsCache)) {
    // Uoh, the steps history is not initialized -> fill it with zero and do it now!
    for(size_t i = 0; i < 7; i++)
      this->_stepsCache[i] = 0;
    prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache));
  } else
    prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache));
  this->_stepsSum = prefs.getUInt(PREFS_STEPS_ALL);
  uint32_t lastDoW = prefs.getUInt(PREFS_STEPS_DAYLASTCHECKED);
  uint32_t currDoM = 0; // Unused, but required by function signature
  uint32_t currDoW = 0;
  this->getDate(&currDoM, &currDoW);
  if(currDoW != lastDoW) {
    const uint32_t currentSteps = this->getAccelStepCount();
    this->_stepsCache[lastDoW] = currentSteps; // write current step to last dow
    this->_stepsSum += currentSteps; // Let's just hope this never rolls over...
    this->resetAccelerometer();
    this->initAccelerometer();
    if(currDoW > lastDoW) {
      // set stepscache to 0 in ]lastDoW, currDoW[
      for(uint32_t i = currDoW; lastDoW + 1 < i; i--)
        this->_stepsCache[i - 1] = 0;
    } else {
      // set > last dow to 0 && set < curr dow to 0
      if(currDoW > 0)
        for(uint32_t i = currDoW; 0 < i; i--)
          this->_stepsCache[i - 1] = 0;
      for(uint32_t i = lastDoW + 1; i < 7; i++)
        this->_stepsCache[i] = 0;
    }
#ifdef DEBUG
    Serial.println(String(__FILE__) + ": Updated steps from DoW " + String(lastDoW) + " to DoW " + String(currDoW));
#endif
    lastDoW = currDoW;
    // write step cache + stepsum
    prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache));
    prefs.putUInt(PREFS_STEPS_ALL, this->_stepsSum);
    prefs.putUInt(PREFS_STEPS_DAYLASTCHECKED, lastDoW);
  }
  prefs.end();
#ifdef DEBUG
    Serial.print(String(__FILE__) + ": Current step history (day " + String(currDoW) + ", sum " + String(this->_stepsSum) + ") is: {");
    for(size_t i = 0; i < 7; i++) {
      if(i > 0)
        Serial.print(", ");
      Serial.print(this->_stepsCache[i]);
    }
    Serial.println("}");
#endif
#endif
}

uint32_t OswHal::getStepsToday(void) {
  return this->getAccelStepCount();
}

#ifdef OSW_FEATURE_STATS_STEPS
uint32_t OswHal::getStepsOnDay(uint8_t dayOfWeek) {
  uint32_t day = 0;
  uint32_t weekday = 0;
  getDate(&day, &weekday);
  if (dayOfWeek == weekday)
    return this->getStepsToday();
  return this->_stepsCache[dayOfWeek];
}
#endif

uint32_t OswHal::getStepsTotal(void) {
#ifdef OSW_FEATURE_STATS_STEPS
  return this->_stepsSum + this->getStepsToday();
#else
  return getStepsToday();
#endif
}