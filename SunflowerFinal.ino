#include <Servo.h>

int leftSensor = 2;
int rightSensor = 1;

int backSensor = 3;
int frontSensor = 0;

int minHorizSpin = 0;
int maxHorizSpin = 180;
float horizSpinSpeed = 2;
int horizLightThreashold = 300;

int minVertiSpin = 65; // this is fully open
int maxVertiSpin = 180; // this is fully closed
float vertiSpinSpeed = 1.5;

int minSolarSpin = 20;
int maxSolarSpin = 110;
float solarSpinSpeed = 1.5;
int solarLightThreashold = 120;

int horizAng = 0;
int horizMax = 110;
int horizMin = 0;
int horizPin = 11;  

// Servos
Servo solarServo; 
Servo vertiServo; 
Servo horizServo; 
int state = 0;
// Pins 
int buttonPin = 12;

const int Opening = 0;
const int Following = 1;
const int Closing = 2;

// 0: Rising
// 1: Following
// 2: Lowering
byte stepCycle;

void setup() {
  Serial.println("Welcome");
  Serial.begin(9600);
  vertiServo.attach(10); 
  solarServo.attach(9); 
  horizServo.attach(11); 
  centerSolarServo();
  delay(5000);
  state = Closing;
}
int buttonState;
int prevButtonState;
void loop() {
  buttonState = digitalRead(buttonPin);
  Serial.println( true ==(prevButtonState == LOW && buttonState == HIGH));
  if(prevButtonState == LOW && buttonState == HIGH){
    // a button is pressed
    state += 1;
    state = state % 3;
  }
  prevButtonState = buttonState;
  Serial.print("State: ");
  Serial.println(state);
  Serial.println("-----------------------------------------");
  switch(state){

    case Following:
      Serial.println("Following");
      followLight();
      break;
    case Opening:
      Serial.println("Opening");
      open_();
      break;
    case Closing:
      Serial.println("Closing");
      close_();
      break;
  }

}

void centerSolarServo(){
  // 65 is the in between point
   for(int i =63; i<=65; i++){
     solarServo.write(i);   
   }
   for(int i =65; i>=63; i--){
     solarServo.write(i);   
   }
}

void followLight(){
  followLightSolar();
  followLightHoriz(); 

}

void followLightSolar(){
  float backValue = analogRead(backSensor);
  float frontValue = analogRead(frontSensor);
  Serial.print("front: ");
  Serial.print(frontValue);
  Serial.print(" back: ");
  Serial.println(backValue);
  
  if(abs(backValue - frontValue) >= solarLightThreashold){
    int change = 0;
    if(max(backValue, frontValue) == backValue){
      change = -solarSpinSpeed;
    }else{
      change = solarSpinSpeed;
    }
    Serial.print("Moving: ");
    Serial.println(change);
    
    int angle = solarServo.read() + change;
    if(angle <= maxSolarSpin || angle >= minSolarSpin){
      // inside range
      solarServo.write(angle);
    }else{
      Serial.println("max or min reached.");
    }
  }
}

boolean isButtonPressed() {
  return digitalRead(buttonPin) == LOW;
}

void detachServos(){
  solarServo.detach();
  horizServo.detach();
}

void attachServos(){
  solarServo.attach(9);
  horizServo.attach(11);
}

void open_() {
  detachServos();
  vertiServo.attach(10);
  for(int pos = vertiServo.read(); pos < minVertiSpin; pos++) {
    vertiServo.write(pos);
    delay(20);
  }
  vertiServo.detach();
  attachServos();
  state = Following;
  delay(2000);
}

void close_(){
  centerSolarServo();
  Serial.println(solarServo.read());
  delay(100);
  detachServos();
  vertiServo.attach(10);
  for(int pos = vertiServo.read(); pos > maxVertiSpin; pos--) {
    vertiServo.write(pos);
    delay(20);
  }
  vertiServo.detach();
  attachServos();
}

void followLightHoriz(){
  float spin1 = analogRead(leftSensor);
  float spin2 = analogRead(rightSensor);
  Serial.print("moving: ");
  Serial.print(spin1); 
  Serial.print(" ");
  Serial.println(spin2 );
  if(abs(spin1 - spin2) >= horizLightThreashold){
    horizServo.write(horizServo.read()+horizSpinSpeed);
    if(horizServo.read() >= maxHorizSpin || horizServo.read() <= minHorizSpin){
      horizSpinSpeed *= -1;
    }
  }
}
