#include "parser.h"
#include "platform.h"
#include <assert.h>

const char* RC_ADDRESS_BASE = "http://192.168.1.";
const char* RC_JSON_ADDRESS_SUFFIX = ":8080/CommandServer/currentJsonCommand";

extern "C" void parse(const char* json, command* wifiRC_command);

#ifndef DESKTOP
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
#else
#include <chrono>
#include <thread>
#include <curl/curl.h>

void delay(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void digitalWrite(int, int)
{
}
size_t curl_write_json_function(void* buffer, size_t size, size_t nmemb, int* p)
{
    set_command command;
    parse((char*)buffer, &command);

    *p = command.get_direction();
    gMagnitude = command.get_magnitude();
    gLights = command.get_lights();
    gGear = command.get_gear();

    return size*nmemb;
}
#endif

namespace platform
{
    void getAddress(char* address_buffer, int size)
    {
        assert(size == 64);

    //Need to do something smarter here like search for servers on the local network
        char buffer[4];
        memset(buffer, '\0', 4);
        memset(address_buffer, '\0', 64);
        strcat(address_buffer, RC_ADDRESS_BASE);
        sprintf(buffer, "%i", DEV);
        strcat(address_buffer, buffer);
        strcat(address_buffer, RC_JSON_ADDRESS_SUFFIX);
    }
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
    const char* getPlatformName()
    {
        return "Intel Edison";
    }
    void send()
    {
        http_request("POST /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\nContent-Length: 15\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nPLATFORM:EDISON\n");
    }
    int getCommand()
    {
//    http_request("GET /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\n");
        return IDLE;
    }
#else
    void setup()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);    
    }
    const char* getPlatformName()
    {
        return "Desktop";
    }
    void send()
    {
        CURL* pCURL = curl_easy_init();
        if(pCURL)
        {
            char address_buffer[64];
            getAddress(address_buffer, 64);
            curl_easy_setopt(pCURL, CURLOPT_URL, address_buffer);
            curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, getPlatformName());
            CURLcode result = curl_easy_perform(pCURL);
            if(result == CURLE_OK)
            {
            }
            curl_easy_cleanup(pCURL);
        }
    }
    int getCommand()
    {
        int command = IDLE;
        CURL* pCURL = curl_easy_init();
        if(pCURL)
        {
            int temp;
            char address_buffer[64];
            getAddress(address_buffer, 64);
            curl_easy_setopt(pCURL, CURLOPT_URL, address_buffer);
            curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, curl_write_json_function);
            curl_easy_setopt(pCURL, CURLOPT_WRITEDATA, &temp);
            CURLcode result = curl_easy_perform(pCURL);
            if(result == CURLE_OK)
            {
                command = temp;
            }
            curl_easy_cleanup(pCURL);
        }

        return command;
    }
#endif
}

