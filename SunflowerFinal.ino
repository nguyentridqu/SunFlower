#include <Servo.h> // include Servo library

Servo horizontal; // horizontal servo
int horizAng = 90; // 90; // stand horizontal servo
int horizMax = 180;
int horizMin = 0;
int horizPin = 11;

Servo vertical; // vertical servo
int vertAng = 50; // 90; // stand vertical servo
int vertMax = 100;
int vertMin = 21;
int vertPin = 9;


// LDR pin connections
// name = analogpin;
const int front_port = 0; //LDR top left - BOTTOM LEFT <--- BDG
const int back_port = 3; //LDR top rigt - BOTTOM RIGHT
const int left_port = 1; //LDR down left - TOP LEFT
const int right_port = 2; //ldr down rigt - TOP RIGHT

int buttonPin = 12;
const int BUTTON_ON = 0;
const int BUTTON_OFF = 1;

// 0: vertical movement only
// 1: horizontal movement only
int stage;
const int VERTICAL_STAGE = 0;
const int HORIZONTAL_STAGE = 1;

void setup() {
  Serial.begin(9600);

  // servo connections
  //   name.attach(pin);

  vertical.attach(vertPin);
  vertical.write(vertAng);
  horizontal.attach(horizPin);
  horizontal.write(horizAng);
  delay(1000);
  horizontal.detach();

  stage = VERTICAL_STAGE;

  delay(100);

  Serial.println("welcome");
}
void loop() {
  //// Cycle stages if needed
  if (isButtonOn()) {
    switch (stage) {
      case VERTICAL_STAGE:
        stage = HORIZONTAL_STAGE;
        break;
      case HORIZONTAL_STAGE:
        stage = VERTICAL_STAGE;
        break;
    }
    Serial.println("Stage cycled");
    delay(1000);
  }

  Serial.print("Button status: ");
  Serial.println(isButtonOn());
  Serial.print("Stage: ");
  Serial.println(stage);

  switch (stage) {
    case VERTICAL_STAGE:

      doVerticalStuff();
      break;
    case HORIZONTAL_STAGE:
      doHorizontalStuff();
      break;
  }
}


boolean isButtonOn() {
  return digitalRead(buttonPin) == BUTTON_ON;
}


void doHorizontalStuff() {
  horizontal.attach(horizPin);
  for (int pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    horizontal.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (int pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    horizontal.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  horizontal.detach();

}

void doVerticalStuff() {

  int back = analogRead(back_port); // top right
  int front = analogRead(front_port); // top left
  int left = analogRead(left_port); // down left
  int right = analogRead(right_port); // down right
  int maxNumber = 0; // store the hightest value as an integer index of vals
  int vals[4];
  vals[0] = front;
  vals[1] = -1;
  vals[2] = -1;
  vals[3] = back;
  Serial.print("F: ");
  Serial.print(front);
  Serial.print(" ");
  Serial.print("B: ");
  Serial.print(back);
  Serial.print(" ");
  Serial.print("L: ");
  Serial.print(left);
  Serial.print(" ");
  Serial.print("R: ");  
  Serial.print(right);
  Serial.println();

  for (int i = 0; i < 4; i++) { // get max number
    if (vals[i] >= maxNumber) {
      maxNumber = vals[i]; // set the index
    }
  }

  Serial.println(maxNumber);
  int horiz_change = 0;
  int vert_change = 0;
  if(back == maxNumber){
      // back greatest
      Serial.println("Back is  the greatest");
      vert_change += 4;
  }else if(front == maxNumber){
      // front greatest
      Serial.println("Front is the greatest");
      vert_change -= 4;
  }else if(left == maxNumber){
      // left greatest
      Serial.println("Left is the greatest");
      horiz_change -= 10; // move towards the greater between (front or back)
  }
    else if (right == maxNumber){
      // right greatest
      horiz_change += 10; // move towards the greater between (front or back)
      Serial.println("Right is the greatest");
  }

  if (vertAng + vert_change > vertMax) {
    vertAng = vertMax;
    vert_change = 0;
    Serial.println("stopping too high");
  } else if (vertAng + vert_change < vertMin) {
    vert_change = 0;
    vertAng = vertMin;
    Serial.println("stopping too low");
  }
  Serial.print("H: ");
  Serial.println(horiz_change);
  if (abs(vals[0] - vals[1]) > 50) {
    vertAng += vert_change;
    Serial.print("Changing by V: ");
    Serial.println(vert_change);
    Serial.println("Angle: ");
    Serial.println(vertAng);
  }
  vertical.write(vertAng);
  Serial.print(vertAng);
  Serial.print("  ");
  Serial.println(vertical.read());
  Serial.println("--------------------------");

}
