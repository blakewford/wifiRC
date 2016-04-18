#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef DESKTOP
#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>
#define DEV 6
#else
#include <chrono>
#include <thread>
#include <curl/curl.h>
#endif

#define RIGHT_ENGAGE_PIN 2
#define STEERING_ENGAGE_PIN 4
#define REVERSE_ENGAGE_PIN 6
#define DRIVE_MOTOR_PIN 8
#define LIGHTS_ENABLE_PIN 10

extern "C"
{
    namespace platform
    {
        void setup();
    }
}

#endif
