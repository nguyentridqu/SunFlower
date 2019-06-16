#include <Servo.h>

int leftSensor = 2;
int rightSensor = 1;

int backSensor = 3;
int frontSensor = 0;

int minHorizSpin = 0;
int maxHorizSpin = 180;
float horizSpinSpeed = 5;
int horizLightThreshold = 50;  //thredshold
int horizDelay = 30;

int minVertiSpin = 65; // this is fully open
int maxVertiSpin = 178; // this is fully closed
float vertiSpinSpeed = 2;

int minSolarSpin = 20;
int maxSolarSpin = 105;
float solarSpinSpeed = 5;
int solarLightThreashold = 120;

// Servos
Servo solarServo;
Servo vertiServo;
Servo horizServo;
int state = 1;

// Pins
int buttonPin = 12;

// Button
int buttonState;
int prevButtonState;

void setup() {
  Serial.println("Welcome");
  Serial.begin(9600);
  vertiServo.attach(10);
  solarServo.attach(6);
  horizServo.attach(11);
  centerSolarServo();
  state = -1;
}

void loop() {
  buttonState = digitalRead(buttonPin);
  Serial.println(true == (prevButtonState == LOW && buttonState == HIGH));
  if (prevButtonState == LOW && buttonState == HIGH) {
    // a button is pressed
    state *= -1;
  }
  prevButtonState = buttonState;
  Serial.print("State: ");
  Serial.println(state);
  Serial.println("-----------------------------------------");
  switch (state) {
    case -1:
      Serial.println("Open");
      open_();
      followLight();
      break;
    case 1:
      Serial.println("Close");
      close_();
      break;
  }

}

void centerSolarServo() {
  // 65 is the in between point
  for (int i = 63; i <= 65; i++) {
    solarServo.write(i);
    delay(10);
  }
  //  for (int i = 65; i >= 63; i--) {
  //    solarServo.write(i);
  //    delay(10);
  //  }
}

void followLight() {
  followLightSolar();
  followLightHoriz();

}

void followLightSolar() {
  float backValue = analogRead(backSensor);
  float frontValue = analogRead(frontSensor);
  Serial.print("front: ");
  Serial.print(frontValue);
  Serial.print(" back: ");
  Serial.println(backValue);

  if (abs(backValue - frontValue) >= solarLightThreashold) {
    int change = 0;
    if (backValue > frontValue) {
      change = -solarSpinSpeed;
    } else {
      change = solarSpinSpeed;
    }
    Serial.print("Moving Solar: ");
    Serial.println(change);

    int angle = solarServo.read() + change;
    if (angle <= maxSolarSpin && angle >= minSolarSpin) {
      // inside range
      solarServo.write(angle);
      delay(20);
    } else {
      Serial.println("max or min reached.");
    }
  }
  Serial.print("Solar Angle: ");
  Serial.println(solarServo.read());
}

boolean isButtonPressed() {
  return digitalRead(buttonPin) == LOW;
}

void detachServos() {
  solarServo.detach();
  horizServo.detach();
}

void attachServos() {
  solarServo.attach(6);
  horizServo.attach(11);
}

void open_() {
  for (int pos = vertiServo.read(); pos > minVertiSpin; pos--) {
    vertiServo.write(pos);
    delay(40);
  }
  if (!horizServo.attached() || !solarServo.attached()) {
    attachServos();
  }
  delay(100);
}

void close_() {
  centerSolarServo();
  delay(100);
  if (horizServo.attached() || solarServo.attached()) {
    detachServos();
  }
  for (int pos = vertiServo.read(); pos < maxVertiSpin; pos++) {
    vertiServo.write(pos);
    delay(40);
  }
}

void followLightHoriz() {
  float spin1 = analogRead(leftSensor);
  float spin2 = analogRead(rightSensor);
  Serial.print("Left: ");
  Serial.print(spin1);
  Serial.print(" Right: ");
  Serial.println(spin2);

  Serial.print("Horizontal Servo: ");
  Serial.println(horizServo.read());
  if (spin1 - spin2 >= horizLightThreshold) {
    horizServo.write(horizServo.read() + horizSpinSpeed);
  } else if (spin2 - spin1 >= horizLightThreshold) {
    horizServo.write(horizServo.read() - horizSpinSpeed);
  }

  if (horizServo.read() <= minHorizSpin ) {
    horizSpinSpeed *= -1;
    horizServo.write(minHorizSpin + 2 * abs(horizSpinSpeed));
  }

  if (horizServo.read() >= maxHorizSpin ) {
    horizSpinSpeed *= -1;
    horizServo.write(maxHorizSpin - 2 * abs(horizSpinSpeed));
  }

}
