/*
#include "parser.h"

#ifndef DESKTOP
#include "mraa.h"
#endif
#include <chrono>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <curl/curl.h>

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

struct context
{
#ifndef DESKTOP
    mraa_gpio_context drive_context;
    mraa_gpio_context reverse_context;
    mraa_gpio_context light_context;
#endif
};

int getCommand();
void getAddress(char* buffer, int size);
void* send(void* params);
void loop(context& gpio_context);

int gMagnitude = 0;
bool gLights = false;
int gGear = 0;
bool gKeepGoing = false;

const char* RC_ADDRESS_BASE = "http://192.168.1.";
const char* RC_JSON_ADDRESS_SUFFIX = ":8080/CommandServer/currentJsonCommand";

#define DRIVE_MOTOR_GPIO 31 //GP44
#define REVERSE_ENGAGE_GPIO 32 //GP46
#define LIGHT_GPIO 33 //GP48
#define DEFAULT_WAIT_TIME_MS 100

extern "C" void parse(const char* json, command* wifiRC_command);

int main(int argc, char** argv)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
#ifndef DESKTOP
    mraa_init();
    mraa_gpio_context drive_context = mraa_gpio_init(DRIVE_MOTOR_GPIO);
    if(drive_context == NULL || mraa_gpio_dir(drive_context, MRAA_GPIO_OUT) != MRAA_SUCCESS) exit(1);
    mraa_gpio_write(drive_context, false);
    mraa_gpio_context reverse_context = mraa_gpio_init(REVERSE_ENGAGE_GPIO);
    if(reverse_context == NULL || mraa_gpio_dir(reverse_context, MRAA_GPIO_OUT) != MRAA_SUCCESS) exit(1);
    mraa_gpio_write(reverse_context, false);
    mraa_gpio_context light_context = mraa_gpio_init(LIGHT_GPIO);
    if(light_context == NULL || mraa_gpio_dir(light_context, MRAA_GPIO_OUT) != MRAA_SUCCESS) exit(1);
    mraa_gpio_write(light_context, false);

#endif

    char platform_name[64];
    memset(platform_name, '\0', 64);
    const char* type = CONTENT_TYPES[0];
#ifndef DESKTOP
    sprintf(platform_name, "%s:%s", type, mraa_get_platform_name());
#else
    sprintf(platform_name, "%s:%s", type, "Desktop");
#endif

    gKeepGoing = true;
    pthread_t send_thread;
    char* delimiter = strchr(platform_name, ':');
    printf("%s Wifi RC Interface\n", delimiter+1);
    pthread_create(&send_thread, NULL, send, platform_name);

    context session;
#ifndef DESKTOP
    session.drive_context = drive_context;
    session.reverse_context = reverse_context;
    session.light_context = light_context;
#endif
    while(true) loop(session);

    gKeepGoing = false;
    pthread_join(send_thread, NULL);
    curl_global_cleanup();
#ifndef DESKTOP
    mraa_deinit();
#endif

    return 0;

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
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return NULL;
}

void loop(context& gpio_context)
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
#ifndef DESKTOP
    mraa_gpio_write(gpio_context.drive_context, shouldDrive);
    mraa_gpio_write(gpio_context.light_context, gLights);
    mraa_gpio_write(gpio_context.reverse_context, gGear);
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(gMagnitude > 0 ? gMagnitude: DEFAULT_WAIT_TIME_MS));
}
*/
