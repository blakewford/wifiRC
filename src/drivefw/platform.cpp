#include "platform.h"

namespace platform
{
#ifndef DESKTOP
    void setup()
    {
        pinMode(LIGHTS_ENABLE_PIN, OUTPUT);
        pinMode(DRIVE_MOTOR_PIN, OUTPUT);
        pinMode(REVERSE_ENGAGE_PIN, OUTPUT);
        pinMode(RIGHT_ENGAGE_PIN, OUTPUT);

        Serial.begin(9600);
        while(status != WL_CONNECTED)
        {
            status = WiFi.begin("", "");
            delay(200);
        }
    }
#else
    void setup()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);    
    }
#endif
}

