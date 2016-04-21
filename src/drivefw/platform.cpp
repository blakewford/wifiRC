#include "parser.h"
#include "platform.h"
#include <assert.h>

extern "C" void parse(const char* json, command* wifiRC_command);

#ifndef DESKTOP
#include "edison_platform.h"
#else
#include "desktop_platform.h"
#endif

#define RC_ADDRESS_BASE "http://192.168.1."
#define RC_JSON_ADDRESS_SUFFIX ":8080/CommandServer/currentJsonCommand"

static void getAddress(char* address_buffer, int size)
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
void platform_setup()
{
    pthread_mutex_init(&gHttpMutex, NULL);

    pinMode(LIGHTS_ENABLE_PIN, OUTPUT);
    pinMode(DRIVE_MOTOR_PIN, OUTPUT);
    pinMode(REVERSE_ENGAGE_PIN, OUTPUT);
    pinMode(RIGHT_ENGAGE_PIN, OUTPUT);

    while(gStatus != WL_CONNECTED)
    {
        gStatus = WiFi.begin("", "");
        delay(DEFAULT_WAIT_TIME_MS);
    }
}
const char* platform_getName()
{
    return "Intel Edison";
}
void platform_send(void* params)
{
    http_request("POST /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\nContent-Length: 15\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nPLATFORM:EDISON\n", NULL, 0);
}
char gJson[128];
char gResponse[256];
int platform_getCommand()
{
    memset(gJson, '\0', 128);
    memset(gResponse, '\0', 256);
    http_request("GET /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\n", gResponse, 256);

    char* start = NULL;
    char* length = NULL;
    start = strchr(gResponse, '\n')+1;
    start = strchr(start, '\n')+1;
    length = strchr(start, ':')+1;
    int size = atoi(length);
    start = strchr(gResponse, '{');
    for(int i = 0; i < size; i++)
    {
        gJson[i] = *start;
        start++;
    }

    set_command command;
    parse(start, &command);
    gMagnitude = command.get_magnitude();
    gLights = command.get_lights();
    gGear = command.get_gear();
    
    return command.get_direction();

}
void platform_cleanup()
{
}
#else
void platform_setup()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}
const char* platform_getName()
{
    return "Desktop";
}
void platform_send(void* params)
{
    CURL* pCURL = curl_easy_init();
    if(pCURL)
    {
        char address_buffer[64];
        getAddress(address_buffer, 64);
        curl_easy_setopt(pCURL, CURLOPT_URL, address_buffer);
        curl_easy_setopt(pCURL, CURLOPT_POSTFIELDS, params);
        CURLcode result = curl_easy_perform(pCURL);
        if(result == CURLE_OK)
        {
        }
        curl_easy_cleanup(pCURL);
    }
}
int platform_getCommand()
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
void platform_cleanup()
{
    curl_global_cleanup();
}
#endif

