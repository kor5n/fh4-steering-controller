#include <Wire.h>
#include <math.h>
//#include <Joystick.h>
#include <HID-Project.h>
#include <HID-Settings.h>

/*Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_MULTI_AXIS,
  4, 0,                  // Button Count, Hat Switch Count
  true, true, true,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  true, true, false);  // No accelerator, brake, or steering
*/
//#define USB_PRODUCT "DIY Racing Wheel"

//int const wheelPin = A0;
int const gasPin = 12;
int const brakePin = 9;
//temporary recalibration
int const recalPin = 17 ;

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

int throttle = 0;

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
  //pinMode(recalPin, INPUT_PULLUP);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  timer = micros();
  recallibration();

  //Joystick.begin();
  //Joystick.setXAxisRange(0, 32767);
  //Joystick.setZAxisRange(0, 32767);

  Gamepad.begin();
  Gamepad.xAxis(0);
  Gamepad.zAxis(0);
  Gamepad.write();
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


  //steeringAngle = analogRead(wheelPin);
  toggleGas = digitalRead(gasPin);
  toggleBrake = digitalRead(brakePin);
  //toggleRecal = digitalRead(recalPin);

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

  //python controller steering version
  //Serial.print("Steering ");
  //Serial.print(steeringAngle);
  //Serial.print("\n");

  if(toggleGas == 0 && toggleBrake == 1){
    throttle = 32767;   // gas
  }
  else if(toggleBrake == 0 && toggleGas == 1){
    throttle = -32767;  // brake/reverse
  }
  else if (toggleBrake == 0 && toggleGas == 0){
    recallibration();
  }
  else {
    throttle = 0;
  }

  

  // HID-Project steering version
  float steer = (angleZ * -200);
  steer = constrain(steer, -32767, 32767);

  if (steer > 32767) steer = 32767;
  if (steer < -32767) steer = -32767;
  if (abs(steer) < 5000) steer = 0;

  Serial.print("Steering ");
  Serial.print((int16_t)steer);

  Serial.print("Throttle ");
  Serial.print(throttle);
  Serial.print("\n");

  // Joystick.h steering version
  //Joystick.setXAxis(angleZ*-1);
  //Joystick.setZAxis(throttle);
  //Joystick.sendState();

  Gamepad.xAxis((int16_t)steer);
  Gamepad.zAxis((int16_t)throttle);
  Gamepad.write();
  delay(1);
}

