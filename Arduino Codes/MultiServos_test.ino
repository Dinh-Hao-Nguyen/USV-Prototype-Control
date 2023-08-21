// Connect up to theoretically 16 servos. (Max number off ledc Channels on the ESP32).

#include <ESP32Servo360.h>

ESP32Servo360 servoL, servoR, servoM;

void setup() {
  pinMode(32,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(35,INPUT);
  pinMode(34,INPUT);
  pinMode(14,INPUT);
  
  servoL.attach(32, 35); // Control pin (white), signal pin (yellow).
  servoR.attach(26, 34);
  servoM.attach(13,14);

  servoL.setSpeed(140);
  servoR.setSpeed(140);
  servoM.setSpeed(140);
}

void loop() {
  servoL.rotateTo(0);
  servoR.rotateTo(0);
  servoM.rotateTo(0);
  servoL.wait();
  servoR.wait();
  servoM.wait();
  servoL.hold();
  servoR.hold();
  servoM.hold();

  delay(3000);

  servoL.release();
  servoR.release();
  servoM.release();
  
  servoL.rotateTo(90);
  servoR.rotateTo(90);
  servoM.rotateTo(90);
  
  servoL.wait();
  servoR.wait();
  servoM.wait();
  
  servoL.hold();
  servoR.hold();
  servoM.hold();
  
  delay(3000);

//  servoL.release();
//  servoR.release();
//  servoM.release();
//
//  servoL.rotateTo(-90);
//  servoR.rotateTo(-90);
//  servoM.rotateTo(-90);
//  
//  servoL.wait();
//  servoR.wait();
//  servoM.wait();
//  
//  servoL.hold();
//  servoR.hold();
//  servoM.hold();
//  
//  delay(3000);
//
//  servoL.release();
//  servoR.release();
//  servoM.release();
}
