/**
 * Calibration of the Turnigy AE-20A ESCs with an ESP32
 * 
 * WIRING:
 *    1. Connect the 3 power-out connectors of the ESC to the BLDC motor.
 *    2. Among the 3 control-in connectors of the ESC:
 *        - connect the ground (black wire) to the GND pin of the ESP32,
 *        - connect the control signal (white wire) to pin 25/26/27 of the ESP32.
 *       No connection is required for the BEC voltage (red wire).
 *    Attention: At this stage, do not power the ESC with the battery! To avoid injuries, also remove the blades from the motor.
 * 
 * CALIBRATION:
 *    1. Plug the ESP32 to your computer with USB cable, open Serial Monitor (9600 baud), then type 2000 to send max throttle to the ESC to enter setup mode.
 *    2. Power up the ESC by connecting the battery. You must hear 2 sets of sounds meaning the power supply is OK and the ESC has entered setup mode.
 *    3. After some seconds, a jingle should play, meaning the throttle highest point has been correctly confirmed.
 *    4. Type 1000 to send min throttle.
 *    5. Again, the jingle should play, which means the throttle lowest point has been correctly confirmed.
 *    6. Then, 6 series of "beep" will start. Each serie corresponds to a function: (1) brake status, (2) electronic timing, (3) battery protection cutoff, (4) aircraft status, (5) throttle response, (6) BEC output voltage.
 *    7. For each serie of "beep", type 1000 (option 1), 1500 (option 2) or 2000 (option 3) to set the desired option:
 *          (1) Brake Status:               1 = brake disabled,       2 = soft brake,                 3 = hard brake
 *          (2) Electronic Timing:          1 = low timing,           2 = mid timing,                 3 = high timing
 *          (3) Battery Protection Cutoff:  1 = high cutoff voltage,  2 = middle cutoff voltage
 *          (4) Aircraft Status:            1 = normal airplane,      2 = helicopter 1 (soft start),  3 = helicopter 2 (soft start + governor mode)
 *          (5) Throttle Response:          1 = standard,             2 = medium speed,               3 = quick speed
 *          (6) BEC Output Voltage:         1 = 5V,                   2 = 5.5V,                       3 = 6V
 *    The setup I have chosen is: 1-2-2-1-1-3.
 * 
 * START-UP:
 *    1. Plug the ESP32 to your computer with USB cable, open Serial Monitor (9600 baud), then type 1000 to send min throttle.
 *    2. Connect the battery to power the ESC. You must hear 2 sets of sounds meaning the power supply is OK and a transmission channel has been detected.
 *    3. Then, 5 series of "beep" should play, corresponding to the option selected for each of the 5 functions (no sound for BEC output voltage function).
 *       In my case, it will be: 1 beep, then 2 beep, then 2 beep, then 1 beep and finally 1 beep.
 * 
 * @author Dinh-Hao NGUYEN
 */

#include <ESP32Servo.h>   // ESP32Servo library installed by Library Manager
#include <ESC.h>          // RC_ESP library installed by Library Manager

// Customize here pulse lengths as needed
#define MIN_SPEED 1000    // minimum pulse length in µs
#define MAX_SPEED 2000    // maximum pulse length in µs

#define ESC_PIN_A 27      // connected to ESC control wire
#define ESC_PIN_B 26
#define ESC_PIN_C 25
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
