#include "mraa.h"
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

int getCommand();
void loop();

int gMagnitude = 0;
const char* RC_ADDRESS = "http://192.168.1.243:8080/CommandServer/currentCommand";

int main(int argc, char** argv)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    printf("%s Wifi RC Interface\n", mraa_get_platform_name());

    while(true) loop();

    curl_global_cleanup();
    mraa_deinit();

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

void loop()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    int raw = getCommand();
    int value = raw;
    if(value > 3)
    {
        value &= 0xC;
    }
    printf("%s %i %i\n", COMMAND_NAMES[value], raw, gMagnitude);
}
