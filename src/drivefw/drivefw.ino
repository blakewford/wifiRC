#define RIGHT_CONTROL_PIN 11
#define RIGHT_ENGAGE_PIN 2
#define STEERING_CONTROL_PIN 3
#define STEERING_ENGAGE_PIN 4
#define REVERSE_CONTROL_PIN 5
#define REVERSE_ENGAGE_PIN 6
#define DRIVE_CONTROL_PIN 7
#define DRIVE_MOTOR_PIN 8
#define LIGHTS_CONTROL_PIN 9
#define LIGHTS_ENABLE_PIN 10

void setup()
{
  pinMode(LIGHTS_ENABLE_PIN, OUTPUT);
  pinMode(LIGHTS_CONTROL_PIN, INPUT_PULLUP);
  pinMode(DRIVE_MOTOR_PIN, OUTPUT);
  pinMode(DRIVE_CONTROL_PIN, INPUT_PULLUP);
  pinMode(REVERSE_ENGAGE_PIN, OUTPUT);
  pinMode(RIGHT_ENGAGE_PIN, OUTPUT);
  pinMode(REVERSE_CONTROL_PIN, INPUT_PULLUP);

  delay(5000); //Make sure the Edison has started running
}

void loop()
{
  if(digitalRead(LIGHTS_CONTROL_PIN) == HIGH) digitalWrite(LIGHTS_ENABLE_PIN, HIGH);
  else digitalWrite(LIGHTS_ENABLE_PIN, LOW);
  
  if(digitalRead(REVERSE_CONTROL_PIN) == HIGH) digitalWrite(REVERSE_ENGAGE_PIN, HIGH);
  else digitalWrite(REVERSE_ENGAGE_PIN, LOW);

  if(digitalRead(REVERSE_CONTROL_PIN) == HIGH) digitalWrite(RIGHT_ENGAGE_PIN, HIGH);
  else digitalWrite(RIGHT_ENGAGE_PIN, LOW);

  if(digitalRead(DRIVE_CONTROL_PIN) == HIGH) digitalWrite(DRIVE_MOTOR_PIN, HIGH);
  else digitalWrite(DRIVE_MOTOR_PIN, LOW);

  delay(10);
}

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

void loop() {
//    http_request("POST /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\nContent-Length: 15\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nPLATFORM:EDISON\n");
//    http_request("GET /CommandServer/currentJsonCommand HTTP/1.1\nHost: 192.168.1.6:8080\nAccept: */*\n");
}
*/



