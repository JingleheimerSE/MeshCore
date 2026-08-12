#pragma once

#ifdef PIN_VIBRATION

#include <Arduino.h>
#include "FindManager.h"

/*
 * Vibration motor control class
 *
 * Provides vibration feedback for events like new messages and new contacts
 * Features:
 * - 1-second vibration pulse
 * - 5-second nag timeout (cooldown between vibrations)
 * - Non-blocking operation
 */

#ifndef VIBRATION_TIMEOUT
#define VIBRATION_TIMEOUT 5000 // 5 seconds default
#endif

class GenericVibration : public FindOutput {
public:
  void begin();       // set up vibration pin
  void trigger();     // trigger vibration if cooldown has passed
  void loop() override; // non-blocking timer handling
  bool isVibrating(); // returns true if currently vibrating
  void stop();        // stop vibration immediately
  void startFind() override;
  void stopFind() override;

private:
  uint32_t duration = 0;
  bool _find_active = false;
  uint32_t _find_phase_origin = 0;
};

#endif // ifdef PIN_VIBRATION
