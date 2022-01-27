
#include <Preferences.h>
#include <osw_hal.h>

#define PREFS_STEPS "PS"
#define PREFS_STEPS_DAYLASTCHECKED "L"
#define PREFS_STEPS_STATS "S"
#define PREFS_STEPS_ALL "A"

void OswHal::Environment::setupStepStatistics() {
#ifdef OSW_FEATURE_STATS_STEPS
  Preferences prefs;
  bool res = prefs.begin(PREFS_STEPS, false); // Open in RW, just in case
  assert(res);
  if(prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) != sizeof(this->_stepsCache)) {
    // Uoh, the steps history is not initialized -> fill it with zero and do it now!
    for(size_t i = 0; i < 7; i++)
      this->_stepsCache[i] = 0;
    res = prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
    assert(res);
  } else {
    res = prefs.getBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
    assert(res);
  }
  this->_stepsSum = prefs.getUInt(PREFS_STEPS_ALL);
  uint32_t lastDoW = prefs.getUInt(PREFS_STEPS_DAYLASTCHECKED);
  uint32_t currDoM = 0; // Unused, but required by function signature
  uint32_t currDoW = 0;
  OswHal::getInstance()->getDate(&currDoM, &currDoW);
  if(currDoW != lastDoW) {
    const uint32_t currentSteps = this->getStepCount_BMA400();
    this->_stepsCache[lastDoW] = currentSteps; // write current step to last dow
    this->_stepsSum += currentSteps; // Let's just hope this never rolls over...
    this->reset_BMA400();
    this->setup_BMA400();
    if(OswConfigAllKeys::stepsHistoryClear.get()) {
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
    }
#ifndef NDEBUG
    Serial.println(String(__FILE__) + ": Updated steps from DoW " + String(lastDoW) + " to DoW " + String(currDoW));
#endif
    lastDoW = currDoW;
    // write step cache + stepsum
    res = prefs.putBytes(PREFS_STEPS_STATS, &this->_stepsCache, sizeof(this->_stepsCache)) == sizeof(this->_stepsCache);
    assert(res);
    res = prefs.putUInt(PREFS_STEPS_ALL, this->_stepsSum) == sizeof(this->_stepsSum);
    assert(res);
    res = prefs.putUInt(PREFS_STEPS_DAYLASTCHECKED, lastDoW) == sizeof(lastDoW);
    assert(res);
  }
  prefs.end();
#ifndef NDEBUG
    Serial.print(String(__FILE__) + ": Current step history (day " + String(currDoW) + ", today " + String(this->getStepCount_BMA400()) + ", sum " + String(this->_stepsSum) + ") is: {");
    for(size_t i = 0; i < 7; i++) {
      if(i > 0)
        Serial.print(", ");
      if(i == currDoW)
        Serial.print("[");
      Serial.print(this->_stepsCache[i]);
      if(i == currDoW)
        Serial.print("]");
    }
    Serial.println("}");
#endif
#endif
}

uint32_t OswHal::Environment::getStepsToday() {
  return this->getStepCount_BMA400();
}

#ifdef OSW_FEATURE_STATS_STEPS
uint32_t OswHal::Environment::getStepsOnDay(uint8_t dayOfWeek) {
  uint32_t day = 0;
  uint32_t weekday = 0;
  OswHal::getInstance()->getDate(&day, &weekday);
  if (dayOfWeek == weekday)
    return this->getStepsToday();
  return this->_stepsCache[dayOfWeek];
}
#endif

uint32_t OswHal::Environment::getStepsTotal() {
#ifdef OSW_FEATURE_STATS_STEPS
  return this->_stepsSum + this->getStepsToday();
#else
  return getStepsToday();
#endif
}