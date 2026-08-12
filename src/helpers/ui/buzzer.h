#pragma once

#include <Arduino.h>
#include <NonBlockingRtttl.h>
#include "FindManager.h"

/* class abstracts underlying RTTTL library 

    Just a simple implementation to start.  At the moment use same
    melody for message and discovery
    Suggest enum type for different sounds
    - on message
    - on discovery

    TODO
    - make message ring tone configurable

*/

class genericBuzzer : public FindOutput
{
    public:
        void begin();  // set up buzzer port
        void play(const char *melody); // Generic play function
        void loop() override;  // loop driven-nonblocking
        void startup();  // play startup sound
        void shutdown();  // play shutdown sound
        bool isPlaying();  // returns true if a sound is still playing else false
        void quiet(bool buzzer_state);  // enables or disables the buzzer
        bool isQuiet();  // get buzzer state on/off

        void startFind() override;  // loops and ignores quiet mode until stopped by FindManager
        void stopFind() override;  // stops an active find pattern and restores prior quiet state

    private:
        // gemini's picks:
        const char *startup_song = "Startup:d=4,o=5,b=160:16c6,16e6,8g6";
        const char *shutdown_song = "Shutdown:d=4,o=5,b=100:8g5,16e5,16c5";
        const char *find_song = "Find:d=8,o=6,b=200:c,e,g,c7";

        bool _is_quiet = true;

        bool _find_active = false;
        bool _find_prev_quiet = false;
};
