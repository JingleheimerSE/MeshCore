#pragma once

#include <MeshCore.h>
#include <helpers/ui/DisplayDriver.h>
#include <helpers/SensorManager.h>
#include <stddef.h>

#include "../AbstractUITask.h"
#include "../DeviceAlerts.h"
#include "../NodePrefs.h"

#include "Button.h"

#ifdef HAS_DRV2605
  #include <helpers/ui/DRV2605Vibration.h>
#endif

class UITask : public AbstractUITask {
  DeviceAlerts* _alerts;
  DisplayDriver* _display;
  SensorManager* _sensors;
#ifdef HAS_DRV2605
  DRV2605Vibration vibration;
#endif
  unsigned long _next_refresh, _auto_off;
  NodePrefs* _node_prefs;
  char _version_info[32];
  char _origin[62];
  char _msg[80];
  char _alert[80];
  int _msgcount;
  bool _need_refresh = true;
  bool _displayWasOn = false;  // Track display state before button press
  unsigned long ui_started_at;

  // Button handlers
#ifdef PIN_USER_BTN
  Button* _userButton = nullptr;
#endif
#ifdef PIN_USER_BTN_ANA
  Button* _userButtonAnalog = nullptr;
#endif

  void renderCurrScreen();
  void userLedHandler();
  void renderBatteryIndicator(uint16_t batteryMilliVolts);
  
  // Button action handlers
  void handleButtonAnyPress();
  void handleButtonShortPress();
  void handleButtonDoublePress();
  void handleButtonTriplePress();
  void handleButtonQuadruplePress();
  void handleButtonLongPress();

 
public:

  UITask(mesh::MainBoard* board, MultiSerialInterface* serial, DeviceAlerts* alerts)
      : AbstractUITask(board, serial), _alerts(alerts), _display(NULL), _sensors(NULL) {
      _next_refresh = 0;
      ui_started_at = 0;
  }
  void begin(DisplayDriver* display, SensorManager* sensors, NodePrefs* node_prefs);

  bool hasDisplay() const { return _display != NULL; }
  void clearMsgPreview();

  // from AbstractUITask
  void msgRead(int msgcount) override;
  void newMsg(uint8_t path_len, const char* from_name, const char* text, int msgcount) override;
  void notify(UIEventType t = UIEventType::none) override;
  void loop() override;

  void shutdown(bool restart = false);
};
