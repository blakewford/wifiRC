#include "parser.h"
#include "platform.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

enum GEAR
{
    D = 0,
    R = 1,
};
const char* GEAR_NAMES[] = {"DRIVE","REVERSE"};

enum COMMAND
{
    IDLE = 0,
    FORWARD = 1,
    REVERSE = 2,
    LEFT = 4,
    RIGHT = 8
};
const char* COMMAND_NAMES[] = {"IDLE","FORWARD","REVERSE","INVALID","LEFT","INVALID","INVALID","INVALID","RIGHT"};

const char* CONTENT_TYPES[] = {"PLATFORM"};

int getCommand();
void getAddress(char* buffer, int size);
void* send(void* params);

int gMagnitude = 0;
bool gLights = false;
int gGear = 0;
bool gKeepGoing = false;
pthread_t gSendThread;

const char* RC_ADDRESS_BASE = "http://192.168.1.";
const char* RC_JSON_ADDRESS_SUFFIX = ":8080/CommandServer/currentJsonCommand";

#define DEFAULT_WAIT_TIME_MS 100

extern "C" void parse(const char* json, command* wifiRC_command);

void setup()
{
    platform::setup();

    char platform_name[64];
    memset(platform_name, '\0', 64);
    const char* type = CONTENT_TYPES[0];
#ifndef DESKTOP
    sprintf(platform_name, "%s:%s", type, "Intel Edison");
#else
    sprintf(platform_name, "%s:%s", type, "Desktop");
#endif

    gKeepGoing = true;
    char* delimiter = strchr(platform_name, ':');
    printf("%s Wifi RC Interface\n", delimiter+1);
    pthread_create(&gSendThread, NULL, send, platform_name);

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

int getCommand()
{
    int command = IDLE;
/*
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
*/
//    http_request("GET /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\n");


    return command;

}

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

void* send(void* params)
{
    while(gKeepGoing)
    {
 /*
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
        //http_request("POST /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\nContent-Length: 15\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nPLATFORM:EDISON\n");
*/
    }

    return NULL;
}

void loop()
{
    int raw = getCommand();

    int value = raw;
    if(value > 3) value &= 0xC;
    printf("%s %s\n", COMMAND_NAMES[value], GEAR_NAMES[gGear]);

    bool shouldDrive = false;
    switch(raw &= 0x3)
    {
        case FORWARD:
        case REVERSE:
            shouldDrive = true;
            break;
        case IDLE:
        default:
            break;
    }
    digitalWrite(DRIVE_MOTOR_PIN, shouldDrive);
    digitalWrite(LIGHTS_ENABLE_PIN, gLights);
    digitalWrite(REVERSE_ENGAGE_PIN, gGear);

    delay(gMagnitude > 0 ? gMagnitude: DEFAULT_WAIT_TIME_MS);
}

#ifdef DESKTOP
int main()
{
    setup();
    while(true)
        loop();

    gKeepGoing = false;
    pthread_join(gSendThread, NULL);
    curl_global_cleanup();

    return 0;
}
#endif

/*
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
  
  delay(100);

  if(client.connected())
  {
    client.stop();
  }
}
*/


