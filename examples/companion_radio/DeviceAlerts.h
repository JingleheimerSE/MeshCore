#pragma once

#include <helpers/ui/FindManager.h>

#ifdef PIN_BUZZER
  #include <helpers/ui/buzzer.h>
#endif
#ifdef PIN_VIBRATION
  #include <helpers/ui/GenericVibration.h>
#endif

class DeviceAlerts {
#ifdef PIN_BUZZER
  genericBuzzer _buzzer;
#endif
#ifdef PIN_VIBRATION
  GenericVibration _vibration;
#endif
  FindManager _find_manager;

public:
  DeviceAlerts() {
#ifdef PIN_BUZZER
    _find_manager.addOutput(_buzzer);
#endif
#ifdef PIN_VIBRATION
    _find_manager.addOutput(_vibration);
#endif
  }

  void begin(bool buzzer_quiet) {
#ifdef PIN_BUZZER
    _buzzer.begin();
    _buzzer.quiet(buzzer_quiet);
#else
    (void)buzzer_quiet;
#endif
#ifdef PIN_VIBRATION
    _vibration.begin();
#endif
  }

  void loop() { _find_manager.loop(); }
  FindManager& findManager() { return _find_manager; }

#ifdef PIN_BUZZER
  genericBuzzer& buzzer() { return _buzzer; }
#endif
#ifdef PIN_VIBRATION
  GenericVibration& vibration() { return _vibration; }
#endif
};
