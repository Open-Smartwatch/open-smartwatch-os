#include "osw_ulp.h"

#ifdef USE_ULP
#include <Arduino.h>
#include <hulp.h>

#define MEAS_INTERVAL_MS 1000

RTC_DATA_ATTR ulp_var_t ulp_tsens_val;
void init_ulp() {
  // Define the ULP Programm here
  const ulp_insn_t program[] = {
      I_TSENS(R0, 1000),
      I_MOVI(R2, 0),
      I_PUT(R0, R2, ulp_tsens_val),
      I_HALT(),
  };
  // hulp_configure_pin(BTN_1, RTC_GPIO_MODE_INPUT_ONLY, GPIO_FLOATING);
  hulp_tsens_configure(3);
  ESP_ERROR_CHECK(hulp_ulp_load(program, sizeof(program), MEAS_INTERVAL_MS * 1000, 0));
  ESP_ERROR_CHECK(hulp_ulp_run(0));
}

void loop_ulp() {
  // Define the code interacting with the ULP program from main loop from here
  Serial.print("TSENS");
  Serial.println(ulp_tsens_val.val);
}
#endif
