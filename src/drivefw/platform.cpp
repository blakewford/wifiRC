#include "platform.h"

#ifndef DESKTOP
WiFiClient client;
IPAddress server(192, 168, 1, DEV);
int status = WL_IDLE_STATUS;
#endif

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

    void delay(int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void digitalWrite(int, int)
    {
    }
#endif
}

