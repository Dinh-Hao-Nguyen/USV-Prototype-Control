#include <ESP32Servo.h>   // ESP32Servo library installed by Library Manager
#include <ESC.h>          // RC_ESP library installed by Library Manager

// Customize here pulse lengths as needed
#define MIN_SPEED 1000    // minimum pulse length in µs
#define MAX_SPEED 2000    // maximum pulse length in µs

#define ESC_PIN_A 25      // connected to ESC control wire
#define ESC_PIN_B 33
#define ESC_PIN_C 27
#define LED_BUILTIN (2)   // not defaulted properly for ESP32s/you must define it

ESC motA(ESC_PIN_A,1000,2000,500);  // ESC_Name(PIN, Minimum Value, Maximum Value, Arm Value)
ESC motB(ESC_PIN_B,1000,2000,500);
ESC motC(ESC_PIN_C,1000,2000,500);

int data;                 // data received from Serial

/**
 * Initialization routine
 */
void setup() {
    Serial.begin(9600);               // set Serial Monitor to 9600 baud
    
    pinMode(ESC_PIN_A,OUTPUT);
    pinMode(ESC_PIN_B,OUTPUT);
    pinMode(ESC_PIN_C,OUTPUT);
    pinMode(LED_BUILTIN,OUTPUT);

    digitalWrite(LED_BUILTIN,HIGH);   // set LED to ON to indicate arming
    motA.arm();                       // send the Arm command to ESC
    motB.arm();
    motC.arm();

    delay(5000);
    digitalWrite(LED_BUILTIN,LOW);    // set LED to OFF to indicate arming completed

    displayInstructions();
}

/**
 * Main function
 */
void loop() {
    if (Serial.available() > 0) {
        data = Serial.parseInt();

            if (data >= MIN_SPEED && data <= MAX_SPEED) {
                Serial.println(data);
                motA.speed(data);
                motB.speed(data);
                motC.speed(data);
            } else {
                Serial.println("INVALID PULSE LENGTH");
            }

    }
//    motC.speed(1140); // middle
//    motB.speed(1000); // right
//    motA.speed(1000); // left
//    delay(5000);
//    motC.speed(1000);
//    motB.speed(1140);
//    motA.speed(1000);
//    delay(5000);
//    motC.speed(1000);
//    motB.speed(1000);
//    motA.speed(1140);
//    delay(5000);
}

/**
 * Displays instructions to user
 */
void displayInstructions() {  
    Serial.print("READY - PLEASE SEND A PULSE LENGTH BETWEEN ");
    Serial.print(MIN_SPEED);
    Serial.print(" AND ");
    Serial.print(MAX_SPEED);
    Serial.println(":");
}
