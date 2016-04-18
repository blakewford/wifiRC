#ifndef DESKTOP
#include <SPI.h>
#include <WiFi.h>
WiFiClient client;
int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,6);
#else
#include <chrono>
#include <thread>
#include <curl/curl.h>
#endif

extern "C"
{
    namespace platform
    {
        void setup();
    }
}

