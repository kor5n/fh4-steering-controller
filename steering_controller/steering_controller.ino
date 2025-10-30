int const wheelPin = A0;
int const gasPin = 2;

int steeringAngle;
int toggleGas;
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

  if(steeringAngle >= 1023/2-diff && steeringAngle <= 1023/2+diff){
    Serial.println("Going forward");
  }else if (steeringAngle > 1023/2+diff){
    Serial.println("Steering right");
  }else if (steeringAngle < 1023/2-diff){
    Serial.println("Steering left");
  }

  if(toggleGas == 1){
    Serial.println("Gas off");
  }else{
    Serial.println("Gas on");
  }
}
