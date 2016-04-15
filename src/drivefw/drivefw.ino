#include <stdio.h>
#include <string.h>

#define RIGHT_ENGAGE_PIN 2
#define STEERING_ENGAGE_PIN 4
#define REVERSE_ENGAGE_PIN 6
#define DRIVE_MOTOR_PIN 8
#define LIGHTS_ENABLE_PIN 10

const char* CONTENT_TYPES[] = {"PLATFORM"};

void setup()
{
#ifndef DESKTOP
    pinMode(LIGHTS_ENABLE_PIN, OUTPUT);
    pinMode(DRIVE_MOTOR_PIN, OUTPUT);
    pinMode(REVERSE_ENGAGE_PIN, OUTPUT);
    pinMode(RIGHT_ENGAGE_PIN, OUTPUT);
#endif

    char platform_name[64];
    memset(platform_name, '\0', 64);
    const char* type = CONTENT_TYPES[0];
#ifndef DESKTOP
    sprintf(platform_name, "%s:%s", type, "Intel Edison");
#else
    sprintf(platform_name, "%s:%s", type, "Desktop");
#endif
}

void loop()
{
#ifndef DESKTOP
    digitalWrite(LIGHTS_ENABLE_PIN, HIGH);
    digitalWrite(LIGHTS_ENABLE_PIN, LOW);
  
    digitalWrite(REVERSE_ENGAGE_PIN, HIGH);
    digitalWrite(REVERSE_ENGAGE_PIN, LOW);

    digitalWrite(RIGHT_ENGAGE_PIN, HIGH);
    digitalWrite(RIGHT_ENGAGE_PIN, LOW);

    digitalWrite(DRIVE_MOTOR_PIN, HIGH);
    digitalWrite(DRIVE_MOTOR_PIN, LOW);

    delay(10);
#endif
}

#ifdef DESKTOP
int main()
{
    setup();
    while(true)
        loop();
    return 0;
}
#endif

/*
#include <SPI.h>
#include <WiFi.h>

WiFiClient client;
int status = WL_IDLE_STATUS;
IPAddress server(192,168,1,6);

void setup()
{
  Serial.begin(9600);
  while(status != WL_CONNECTED)
  {  
    status = WiFi.begin(ssid, pass);
    delay(200);
  } 
}

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
//void loop() {
//    http_request("POST /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\nContent-Length: 15\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nPLATFORM:EDISON\n");
//    http_request("GET /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\n");
//}




