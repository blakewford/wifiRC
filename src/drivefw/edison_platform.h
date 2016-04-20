#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>
#define DEV 6

WiFiClient gClient;
IPAddress gServer(192, 168, 1, DEV);
int gStatus = WL_IDLE_STATUS;
pthread_mutex_t gHttpMutex;

void http_request(const char* request)
{
    pthread_mutex_lock(&gHttpMutex);
    if(gClient.connect(gServer, 8080))
    {
        gClient.println(request);

        while(gClient.available())
        {
            char c = gClient.read();
        }
    }

    if(gClient.connected())
    {
        gClient.stop();
    }
    pthread_mutex_unlock(&gHttpMutex);
}
