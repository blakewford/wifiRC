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
const char* RC_ADDRESS_SUFFIX = ":8080/CommandServer/currentCommand";
const char* RC_JSON_ADDRESS_SUFFIX = ":8080/CommandServer/currentJsonCommand";

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

static const char* directionTerminal = "direction";
static const char* magnitudeTerminal = "magnitude";
static const char* lightsTerminal = "lights";
static const char* gearTerminal = "gear";
size_t curl_write_function(void* buffer, size_t size, size_t nmemb, int* p)
{
    static const char* NAME_STRING = "Name";
    static const char* VALUE_STRING = "Value";
    static const char* RESPONSE_STRING = "Response";
    static const char* TERMINAL_STRING = "Terminal";

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
        if(!strcmp(name, lightsTerminal))
        {
            gLights = atoi(value);
        }
        if(!strcmp(name, gearTerminal))
        {
            gGear = atoi(value);
        }
    }

    return size*nmemb;
}

size_t curl_write_json_function(void* buffer, size_t size, size_t nmemb, int* p)
{
    static const char* WEAVE_DEVICE = "_wifiRC";
    static const char* SET_COMMAND = "set";
    static const char* NAME_STRING = " \"name\"";
    static const char* PARAMETER_STRING = " \"parameters\"";
    const char* weave_command[] = {NAME_STRING, PARAMETER_STRING};

    char line[64];
    char name[64];
    char value[64];
    char* parse = strstr((char*)buffer, "{")+strlen("{")+1;

    int length = 0;
    int data_start = 0;
    for(int i = 0; i < 2; i++)
    {
        memset(line, '\0', 64);
        memset(name, '\0', 64);
        memset(value, '\0', 64);
        length = strchr(parse, '\n')-parse;
        memcpy(line, parse, length);
        data_start = strchr(line, ':')-line;
        memcpy(name, line, data_start);
        assert(!strcmp(weave_command[i], name));
        if(!strcmp(name, NAME_STRING))
        {
            char command[64];
            char application[64];
            char* command_start = 0;
            memset(command, '\0', 64);
            memset(application, '\0', 64);
            memcpy(value, line+data_start+3, length-data_start-5);
            command_start = strchr(value, '.');
            memcpy(application, value, command_start-value);
            assert(!strcmp(application, WEAVE_DEVICE));
            memcpy(command, command_start+1, strlen(value)-strlen(application)-1);
            assert(!strcmp(command, SET_COMMAND));
        }
        if(!strcmp(name, PARAMETER_STRING))
        {
            //Do nothing!
        }
        parse+=length+1;
    }
    while(strcmp(parse, " }\n}\n"))
    {
        length = strchr(parse, '\n')-parse;
        memset(name, '\0', 64);
        memcpy(name, parse, length);
        printf("%s\n", name);
        parse+=length+1;
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
        char address_buffer[64];
        getAddress(address_buffer, 64);
        curl_easy_setopt(pCURL, CURLOPT_URL, address_buffer);
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
    strcat(address_buffer, RC_ADDRESS_SUFFIX);
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
