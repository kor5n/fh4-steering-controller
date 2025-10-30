int const wheelPin = A0;
int const gasPin = 2;
int const brakePin = 4;

int steeringAngle;
int toggleGas;
int toggleBrake;
int gas = false;
const int diff = 200;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(gasPin, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  steeringAngle = analogRead(wheelPin);
  toggleGas = digitalRead(gasPin);
  toggleBrake = digitalRead(brakePin);

  if(steeringAngle >= 1023/2-diff && steeringAngle <= 1023/2+diff){
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
  }

  if(toggleGas == 0){
    Serial.println("Gas on");
  }else{
    Serial.println("Gas off");
  }

  /*Testing brake button
  Serial.println(toggleBrake);
  Serial.print("G:");
  Serial.print(toggleGas);
  Serial.print("\n");*/
  if (toggleBrake == 0){
    Serial.println("Brake turn");
  }else{
    Serial.println("Brake crap");
  }
}
