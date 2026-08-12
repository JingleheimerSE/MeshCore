#include "Arduino.h"
#ifdef PIN_BUZZER
#include "buzzer.h"

void genericBuzzer::begin() {
//    Serial.print("DBG: Setting up buzzer on pin ");
//    Serial.println(PIN_BUZZER);
    #ifdef PIN_BUZZER_EN
      pinMode(PIN_BUZZER_EN, OUTPUT);
      digitalWrite(PIN_BUZZER_EN, HIGH);
    #endif

    quiet(false);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW); // need to pull low by default to avoid extreme power draw
}

void genericBuzzer::play(const char *melody) {
    if (isPlaying())   // interrupt existing
    {
        rtttl::stop();
    }

    if (_is_quiet) return;

    rtttl::begin(PIN_BUZZER,melody);
//    Serial.print("DBG: Playing melody - isQuiet: ");
//    Serial.println(isQuiet());
}

bool genericBuzzer::isPlaying() {
    return rtttl::isPlaying();
}

void genericBuzzer::loop() {
    if (_find_active) {
        if (rtttl::done()) {
            rtttl::begin(PIN_BUZZER, find_song); // repeat until FindManager calls stopFind()
        } else {
            rtttl::play();
        }
        return;
    }

    if (!rtttl::done()) rtttl::play();
}

void genericBuzzer::startup() {
    play(startup_song);
}

void genericBuzzer::shutdown() {
    play(shutdown_song);
}

void genericBuzzer::quiet(bool buzzer_state) {
    _is_quiet = buzzer_state;
#ifdef PIN_BUZZER_EN
    if (_is_quiet) {
      digitalWrite(PIN_BUZZER_EN, LOW);
    } else {
      digitalWrite(PIN_BUZZER_EN, HIGH);
    }
#endif
}

bool genericBuzzer::isQuiet() {
    return _is_quiet;
}

void genericBuzzer::startFind() {
    if (isPlaying()) {
        rtttl::stop();
    }

    // Find must be audible even if the device is muted -- a muted lost
    // device would otherwise be unfindable. Bypass quiet mode for the
    // duration and restore it in stopFind().
    if (!_find_active) {
        _find_prev_quiet = _is_quiet;
    }
    quiet(false);

    _find_active = true;
    rtttl::begin(PIN_BUZZER, find_song);
}

void genericBuzzer::stopFind() {
    if (!_find_active) return;

    rtttl::stop();
    _find_active = false;
    quiet(_find_prev_quiet);
}

#endif  // ifdef PIN_BUZZER
