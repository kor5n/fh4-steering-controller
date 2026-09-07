#include <Wire.h>
#include <math.h>
#include <XInput.h>

// Pins
int const gasPin = 12;
int const brakePin = 9;
// int const recalPin = 17; // still unused, kept for reference

int toggleGas;
int toggleBrake;

// Gyro (MPU6050) setup
const int MPU = 0x68;
float angleZ = 0.0;
unsigned long timer;

float biasZ = 0;
int samples = 500;

// XInput uses separate 0-255 triggers instead of one signed throttle axis,
// so gas and brake are tracked independently.
uint8_t gasValue = 0;
uint8_t brakeValue = 0;

// LED fallback (no Serial needed): off = centered, slow blink = right, fast blink = left
const int ledPin = LED_BUILTIN;
unsigned long lastBlinkTime = 0;
bool ledState = false;

void recallibration()
{
  long sum = 0;
  for (int i = 0; i < samples; i++)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    Wire.read(); Wire.read();
    Wire.read(); Wire.read();
    int16_t rawZ = Wire.read() << 8 | Wire.read();
    sum += rawZ;
    delay(2);
  }
  biasZ = sum / (float)samples;
  angleZ = 0; // re-center steering after recalibration, or drift persists
  Serial.print("Bias ");
  Serial.println(biasZ);
}

void setup() {
  Serial.begin(115200);
  pinMode(gasPin, INPUT_PULLUP);
  pinMode(brakePin, INPUT_PULLUP);
  // pinMode(recalPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  timer = micros();
  recallibration();

  // XInput setup (replaces Gamepad.begin()/xAxis()/zAxis()/write())
  XInput.begin();
}

void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x47); // start at GYRO_ZOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 2, true);

  int16_t gyroZ = Wire.read() << 8 | Wire.read();
  float Gz = (gyroZ - biasZ) / 131.0;

  unsigned long now = micros();
  float dt = (now - timer) / 1000000.0;
  timer = now;

  angleZ += (Gz * dt);

  toggleGas = digitalRead(gasPin);
  toggleBrake = digitalRead(brakePin);

  if (toggleGas == 0 && toggleBrake == 1) {
    gasValue = 255;   // full throttle, on right trigger
    brakeValue = 0;
  }
  else if (toggleBrake == 0 && toggleGas == 1) {
    gasValue = 0;
    brakeValue = 255; // full brake, on left trigger
  }
  else if (toggleBrake == 0 && toggleGas == 0) {
    recallibration();
    gasValue = 0;
    brakeValue = 0;
  }
  else {
    gasValue = 0;
    brakeValue = 0;
  }

  // Steering, mapped onto the left stick's X axis
  float steer = (angleZ * -250);
  steer = constrain(steer, -32767, 32767);
  if (abs(steer) < 5000) steer = 0; // deadzone

  Serial.print("Steering ");
  Serial.print((int16_t)steer);
  Serial.print(" Gas ");
  Serial.print(gasValue);
  Serial.print(" Brake ");
  Serial.print(brakeValue);
  Serial.print("\n");

  XInput.setJoystickX(JOY_LEFT, (int16_t)steer);
  XInput.setTrigger(TRIGGER_RIGHT, gasValue);
  XInput.setTrigger(TRIGGER_LEFT, brakeValue);

  // LED fallback: off = centered, slow blink = right, fast blink = left
  unsigned long millisNow = millis();
  if (steer == 0) {
    digitalWrite(ledPin, LOW);
  }
  else if (steer > 0) {
    if (millisNow - lastBlinkTime >= 500) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastBlinkTime = millisNow;
    }
  }
  else {
    if (millisNow - lastBlinkTime >= 100) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      lastBlinkTime = millisNow;
    }
  }

  //delay(1);
}
