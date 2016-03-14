#define EDISON_CONTROL_PIN 12
#define DRIVE_MOTOR_PIN 13

void setup()
{
  pinMode(EDISON_CONTROL_PIN, INPUT_PULLUP);
  pinMode(DRIVE_MOTOR_PIN, OUTPUT);

  delay(5000); //Make sure the Edison has started running
}

void loop()
{
  if(digitalRead(EDISON_CONTROL_PIN) == HIGH) digitalWrite(DRIVE_MOTOR_PIN, HIGH);
  else digitalWrite(DRIVE_MOTOR_PIN, LOW);

  delay(10);
}
