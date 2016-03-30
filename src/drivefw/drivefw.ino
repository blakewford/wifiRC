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
