#define REVERSE_CONTROL_PIN 10
#define REVERSE_ENGAGE_PIN 11
#define DRIVE_CONTROL_PIN 12
#define DRIVE_MOTOR_PIN 13

void setup()
{
  pinMode(DRIVE_MOTOR_PIN, OUTPUT);
  pinMode(DRIVE_CONTROL_PIN, INPUT_PULLUP);
  pinMode(REVERSE_ENGAGE_PIN, OUTPUT);
  pinMode(REVERSE_CONTROL_PIN, INPUT_PULLUP);

  delay(5000); //Make sure the Edison has started running
}

void loop()
{
  if(digitalRead(REVERSE_CONTROL_PIN) == HIGH) digitalWrite(REVERSE_ENGAGE_PIN, HIGH);
  else digitalWrite(REVERSE_ENGAGE_PIN, LOW);

  if(digitalRead(DRIVE_CONTROL_PIN) == HIGH) digitalWrite(DRIVE_MOTOR_PIN, HIGH);
  else digitalWrite(DRIVE_MOTOR_PIN, LOW);

  delay(1000);
}
