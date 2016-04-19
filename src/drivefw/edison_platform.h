#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>
#define DEV 6

WiFiClient client;
IPAddress server(192, 168, 1, DEV);
int status = WL_IDLE_STATUS;

void http_request(const char* request)
{
    if(client.connect(server, 8080))
    {
        client.println(request);

        while (client.available())
        {
            char c = client.read();
        }
    }

    delay(DEFAULT_WAIT_TIME_MS);

    if(client.connected())
    {
        client.stop();
    }
}
