#define REVERSE_CONTROL_PIN 5
#define REVERSE_ENGAGE_PIN 6
#define DRIVE_CONTROL_PIN 7
#define DRIVE_MOTOR_PIN 8

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
