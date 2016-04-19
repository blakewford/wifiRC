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
static size_t curl_write_json_function(void* buffer, size_t size, size_t nmemb, int* p)
{
    set_command command;
    parse((char*)buffer, &command);

    *p = command.get_direction();
    gMagnitude = command.get_magnitude();
    gLights = command.get_lights();
    gGear = command.get_gear();

    return size*nmemb;
}
