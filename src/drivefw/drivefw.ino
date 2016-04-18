#include "platform.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

enum GEAR
{
    D = 0,
    R = 1,
};
const char* GEAR_NAMES[] = {"DRIVE","REVERSE"};

const char* COMMAND_NAMES[] = {"IDLE","FORWARD","REVERSE","INVALID","LEFT","INVALID","INVALID","INVALID","RIGHT"};
const char* CONTENT_TYPES[] = {"PLATFORM"};

void* post(void* params);

int gMagnitude = 0;
bool gLights = false;
int gGear = 0;
bool gKeepGoing = false;
pthread_t gSendThread;

void setup()
{
    platform::setup();

    char platform_name[64];
    memset(platform_name, '\0', 64);
    const char* type = CONTENT_TYPES[0];
    sprintf(platform_name, "%s:%s", type, platform::getPlatformName());

    gKeepGoing = true;
    char* delimiter = strchr(platform_name, ':');
    printf("%s Wifi RC Interface\n", delimiter+1);
    pthread_create(&gSendThread, NULL, post, platform_name);

}

void* post(void* params)
{
    while(gKeepGoing)
    {
        platform::send();
        delay(1000);
    }

    return NULL;
}

void loop()
{
    int raw = platform::getCommand();

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
    //curl_global_cleanup();

    return 0;
}
#endif


