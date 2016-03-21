#ifndef DESKTOP
#include "mraa.h"
#endif
#include <chrono>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

enum COMMAND
{
    IDLE = 0,
    FORWARD = 1,
    REVERSE = 2,
    LEFT = 4,
    RIGHT = 8
};
const char* COMMAND_NAMES[] = {"IDLE","FORWARD","REVERSE","INVALID","LEFT","INVALID","INVALID","INVALID","RIGHT"};

struct context
{
#ifndef DESKTOP
    mraa_gpio_context drive_context;
    mraa_gpio_context reverse_context;
    mraa_gpio_context light_context;
#endif
};

int getCommand();
void loop(context& gpio_context);

int gMagnitude = 0;
bool gReverseEnabled = false;
const char* RC_ADDRESS = "http://192.168.1.243:8080/CommandServer/currentCommand";

#define DRIVE_MOTOR_GPIO 31 //GP44
#define REVERSE_ENGAGE_GPIO 32 //GP46
#define LIGHT_GPIO 33 //GP48
#define DEFAULT_WAIT_TIME_MS 300

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

    printf("%s Wifi RC Interface\n", mraa_get_platform_name());
#endif

    context session;
#ifndef DESKTOP
    session.drive_context = drive_context;
    session.reverse_context = reverse_context;
    session.light_context = light_context;
#endif
    while(true) loop(session);

    curl_global_cleanup();
#ifndef DESKTOP
    mraa_deinit();
#endif

    return 0;

}

size_t curl_write_function(void* buffer, size_t size, size_t nmemb, int* p)
{
    static const char* NAME_STRING = "Name";
    static const char* VALUE_STRING = "Value";
    static const char* RESPONSE_STRING = "Response";
    static const char* TERMINAL_STRING = "Terminal";
    static const char* directionTerminal = "direction";
    static const char* magnitudeTerminal = "magnitude";

    char test[64];
    char name[64];
    char value[64];
    char* parse = strstr((char*)buffer, RESPONSE_STRING)+strlen(RESPONSE_STRING)+1;
    memset(test, '\0', 64);
    memcpy(test, parse+3, strlen(RESPONSE_STRING));
    while(strcmp(test, RESPONSE_STRING))
    {
        parse = strstr(parse, TERMINAL_STRING)+strlen(TERMINAL_STRING)+1;
        parse = strstr(parse, NAME_STRING)+strlen(NAME_STRING)+1;
        int length = strchr(parse, '<')-parse;
        memset(name, '\0', 64);
        memcpy(name, parse, length);
        parse = strstr(parse, VALUE_STRING)+strlen(VALUE_STRING)+1;
        length = strchr(parse, '<')-parse;
        memset(value, '\0', 64);
        memcpy(value, parse, length);
        parse = strstr(parse, TERMINAL_STRING)+strlen(TERMINAL_STRING)+1;
        memset(test, '\0', 64);
        memcpy(test, parse+3, strlen(RESPONSE_STRING));
        if(!strcmp(name, directionTerminal))
        {
            *p = atoi(value);
        }
        if(!strcmp(name, magnitudeTerminal))
        {
            gMagnitude = atoi(value);
        }
    }

    return size*nmemb;
}

int getCommand()
{
    int command = IDLE;
    CURL* pCURL = curl_easy_init();
    if(pCURL)
    {
        int temp;
        curl_easy_setopt(pCURL, CURLOPT_URL, RC_ADDRESS);
        curl_easy_setopt(pCURL, CURLOPT_WRITEFUNCTION, curl_write_function);
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

void enableReverse(bool enabled, context& gpio_context)
{
#ifndef DESKTOP
    mraa_gpio_write(gpio_context.drive_context, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_TIME_MS));
    mraa_gpio_write(gpio_context.reverse_context, enabled);
    gReverseEnabled = enabled;
#endif
}

void loop(context& gpio_context)
{
    int raw = getCommand();

    int value = raw;
    if(value > 3) value &= 0xC;
    printf("%s\n", COMMAND_NAMES[value]);

    bool killReverse = true;
    bool shouldDrive = false;
    switch(raw &= 0x3)
    {
        case FORWARD:
            shouldDrive = true;
            break;
        case REVERSE:
            killReverse = false;
            if(!gReverseEnabled) enableReverse(true, gpio_context);
            shouldDrive = true;
            break;
        case IDLE:
        default:
            break;
    }
    if(gReverseEnabled && killReverse) enableReverse(false, gpio_context);
#ifndef DESKTOP
    mraa_gpio_write(gpio_context.drive_context, shouldDrive);
#endif
    std::this_thread::sleep_for(std::chrono::milliseconds(gMagnitude > 0 ? gMagnitude: DEFAULT_WAIT_TIME_MS));
}
