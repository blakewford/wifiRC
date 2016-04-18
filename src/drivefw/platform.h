#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef DESKTOP
void delay(int ms);
void digitalWrite(int, int);
#endif

#define RIGHT_ENGAGE_PIN 2
#define STEERING_ENGAGE_PIN 4
#define REVERSE_ENGAGE_PIN 6
#define DRIVE_MOTOR_PIN 8
#define LIGHTS_ENABLE_PIN 10

#define DEFAULT_WAIT_TIME_MS 100

enum COMMAND
{
    IDLE = 0,
    FORWARD = 1,
    REVERSE = 2,
    LEFT = 4,
    RIGHT = 8
};

extern int gMagnitude;
extern bool gLights;
extern int gGear;

extern "C"
{
    namespace platform
    {
        void setup();
        const char* getPlatformName();
        void send();
        int getCommand();
    }
}

#endif
