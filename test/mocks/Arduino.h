#pragma once

#include <cstdint>
#include <cmath>
#include "Stream.h"

inline uint32_t g_mock_millis = 0;
inline int g_mock_pin_mode_pin = -1;
inline int g_mock_pin_mode_value = -1;
inline int g_mock_digital_write_pin = -1;
inline int g_mock_digital_write_value = -1;
inline int g_mock_digital_write_count = 0;

constexpr int OUTPUT = 1;
constexpr int LOW = 0;
constexpr int HIGH = 1;

using std::isnan;

inline uint32_t millis() {
  return g_mock_millis;
}

inline void delay(uint32_t ms) {
  g_mock_millis += ms;
}

inline void pinMode(int pin, int mode) {
  g_mock_pin_mode_pin = pin;
  g_mock_pin_mode_value = mode;
}

inline void digitalWrite(int pin, int value) {
  g_mock_digital_write_pin = pin;
  g_mock_digital_write_value = value;
  g_mock_digital_write_count++;
}

inline void resetArduinoMocks() {
  g_mock_millis = 0;
  g_mock_pin_mode_pin = -1;
  g_mock_pin_mode_value = -1;
  g_mock_digital_write_pin = -1;
  g_mock_digital_write_value = -1;
  g_mock_digital_write_count = 0;
}
