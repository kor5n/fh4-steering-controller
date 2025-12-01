#include <Wire.h>
#include <math.h>

//int const wheelPin = A0;
int const gasPin = 2;
int const brakePin = 4;
//temporary recalibration
int const recalPin = 3;

//int steeringAngle;
int toggleGas;
int toggleBrake;
int toggleRecal;
int gas = false;
const int diff = 200;

//sensor
const int MPU = 0x68;
float angleZ = 0.0;
unsigned long timer;

//tmp
float biasZ = 0;
int samples = 500;

//temporary
void recallibration()
{
  long sum = 0;
  for (int i = 0; i< samples; i++)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    Wire.read(); Wire.read();
    Wire.read(); Wire.read();
    int16_t rawZ = Wire.read () << 8 | Wire.read();
    sum += rawZ;
    delay(2);
  }
  biasZ = sum / (float)samples;
  Serial.print("Bias ");
  Serial.println(biasZ);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(gasPin, INPUT_PULLUP);
  pinMode(brakePin, INPUT_PULLUP);
  //tmp
  pinMode(recalPin, INPUT_PULLUP);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  timer = micros();
  recallibration();
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

  Serial.print("Steering ");
  Serial.println(angleZ+90);

  //steeringAngle = analogRead(wheelPin);
  toggleGas = digitalRead(gasPin);
  toggleBrake = digitalRead(brakePin);
  toggleRecal = digitalRead(recalPin);

  //keyboard steering version
  /*if(steeringAngle >= 1023/2-diff && steeringAngle <= 1023/2+diff){
    Serial.println("Steering forward");
  }else if (steeringAngle > 1023/2+diff){
    if (steeringAngle > 1023/2+2*diff){
      Serial.println("Steering right full");
    }else if(steeringAngle <= 1023/2+2*diff){
      Serial.println("Steering right easy");
    }
    //Serial.println("Steering right");
    
  }else if (steeringAngle < 1023/2-diff){
    if (steeringAngle < 1023/2-2*diff){
      Serial.println("Steering left full");
    }else if(steeringAngle >= 1023/2-2*diff){
      Serial.println("Steering left easy");
    }
    //Serial.println("Steering left");
  }*/

  //controller steering version
  //Serial.print("Steering ");
  //Serial.print(steeringAngle);
  //Serial.print("\n");

  if(toggleGas == 0){
    Serial.println("Gas on");
  }else{
    Serial.println("Gas off");
  }

  if (toggleBrake == 0){
    Serial.println("Brake turn");
  }else{
    Serial.println("Brake crap");
  }

  if (toggleRecal == 0)
  {
    recallibration();
  }

  delay(2);
}

