#pragma once

#include <Arduino.h>

class FindOutput {
public:
  virtual ~FindOutput() {}
  virtual void startFind() = 0;
  virtual void stopFind() = 0;
  virtual void loop() = 0;
};

class FindStateListener {
public:
  virtual ~FindStateListener() {}
  virtual void onFindStateChanged(bool active) = 0;
};

class FindManager {
  static const uint8_t MAX_OUTPUTS = 4;
  static const uint32_t FIND_CAP_MS = 30000;

  FindOutput* _outputs[MAX_OUTPUTS] = {};
  uint8_t _output_count = 0;
  bool _active = false;
  uint32_t _started_at = 0;
  FindStateListener* _listener = nullptr;

  void setActive(bool active) {
    if (_active == active) return;
    _active = active;
    if (_listener) _listener->onFindStateChanged(active);
  }

public:
  bool addOutput(FindOutput& output) {
    for (uint8_t i = 0; i < _output_count; i++) {
      if (_outputs[i] == &output) return true;
    }
    if (_output_count >= MAX_OUTPUTS) return false;
    _outputs[_output_count++] = &output;
    return true;
  }

  void setListener(FindStateListener* listener) { _listener = listener; }
  bool isSupported() const { return _output_count > 0; }
  bool isActive() const { return _active; }

  bool start() {
    if (!isSupported()) return false;
    for (uint8_t i = 0; i < _output_count; i++) {
      _outputs[i]->startFind();
    }
    _started_at = millis();
    setActive(true);
    return true;
  }

  bool stop() {
    if (!isSupported()) return false;
    for (uint8_t i = 0; i < _output_count; i++) {
      _outputs[i]->stopFind();
    }
    setActive(false);
    return true;
  }

  void loop() {
    if (_active && (uint32_t)(millis() - _started_at) >= FIND_CAP_MS) {
      stop();
    }
    for (uint8_t i = 0; i < _output_count; i++) {
      _outputs[i]->loop();
    }
  }
};
