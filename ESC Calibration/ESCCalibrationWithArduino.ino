/**
 * Calibration of the Turnigy AE-20A ESCs with an Arduino Due
 * 
 * WIRING:
 *    1. Connect the 3 power-out connectors of the ESC to the BLDC motor.
 *    2. Among the 3 control-in connectors of the ESC:
 *        - connect the ground (black wire) to the GND pin of the Arduino,
 *        - connect the control signal (white wire) to pin 4/5/6/7 of the Arduino.
 *       No connection is required for the BEC voltage (red wire).
 *    Attention: At this stage, do not power the ESC with the battery! To avoid injuries, also remove the blades from the motor.
 * 
 * CALIBRATION:
 *    1. Plug the Arduino to your computer with USB cable, open Serial Monitor (9600 baud), then type 3 to send max throttle to the ESC to enter setup mode.
 *    2. Power up the ESC by connecting the battery. You must hear 2 sets of sounds meaning the power supply is OK and the ESC has entered setup mode.
 *    3. After some seconds, a jingle should play, meaning the throttle highest point has been correctly confirmed.
 *    4. Type 1 to send min throttle.
 *    5. Again, the jingle should play, which means the throttle lowest point has been correctly confirmed.
 *    6. Then, 6 series of "beep" will start. Each serie corresponds to a function: (1) brake status, (2) electronic timing, (3) battery protection cutoff, (4) aircraft status, (5) throttle response, (6) BEC output voltage.
 *    7. For each serie of "beep", type 1, 2 or 3 to set the desired option:
 *          (1) Brake Status:               1 = brake disabled,       2 = soft brake,                 3 = hard brake
 *          (2) Electronic Timing:          1 = low timing,           2 = mid timing,                 3 = high timing
 *          (3) Battery Protection Cutoff:  1 = high cutoff voltage,  2 = middle cutoff voltage
 *          (4) Aircraft Status:            1 = normal airplane,      2 = helicopter 1 (soft start),  3 = helicopter 2 (soft start + governor mode)
 *          (5) Throttle Response:          1 = standard,             2 = medium speed,               3 = quick speed
 *          (6) BEC Output Voltage:         1 = 5V,                   2 = 5.5V,                       3 = 6V
 *    The setup I have chosen is: 1-2-2-1-1-3.
 * 
 * START-UP:
 *    1. Plug the Arduino to your computer with USB cable, open Serial Monitor (9600 baud), then type 1 to send min throttle.
 *    2. Connect the battery to power the ESC. You must hear 2 sets of sounds meaning the power supply is OK and a transmission channel has been detected.
 *    3. Then, 5 series of "beep" should play, corresponding to the option selected for each of the 5 functions (no sound for BEC output voltage function).
 *       In my case, it will be: 1 beep, then 2 beep, then 2 beep, then 1 beep and finally 1 beep.
 * 
 * @author Dinh-Hao NGUYEN
 */

#include <Servo.h>

// Customize here pulse lengths as needed
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

#define MID_PULSE_LENGTH (MIN_PULSE_LENGTH + MAX_PULSE_LENGTH)/2

Servo motA, motB, motC, motD;
char data;

/**
 * Initialization routine
 */
void setup() {
    Serial.begin(9600); // Set Serial Monitor to 9600 baud
    
    motA.attach(4, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motB.attach(5, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motC.attach(6, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motD.attach(7, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    
    displayInstructions();
}

/**
 * Main function
 */
void loop() {
    if (Serial.available()) {
        data = Serial.read();

        switch (data) {
            // 1
            case 49 : Serial.println("Sending minimum throttle");
                      motA.writeMicroseconds(MIN_PULSE_LENGTH);
                      motB.writeMicroseconds(MIN_PULSE_LENGTH);
                      motC.writeMicroseconds(MIN_PULSE_LENGTH);
                      motD.writeMicroseconds(MIN_PULSE_LENGTH);
            break;

            // 2
            case 50 : Serial.println("Sending medium throttle");
                      motA.writeMicroseconds(MID_PULSE_LENGTH);
                      motB.writeMicroseconds(MID_PULSE_LENGTH);
                      motC.writeMicroseconds(MID_PULSE_LENGTH);
                      motD.writeMicroseconds(MID_PULSE_LENGTH);
            break;

            // 3
            case 51 : Serial.println("Sending maximum throttle");
                      motA.writeMicroseconds(MAX_PULSE_LENGTH);
                      motB.writeMicroseconds(MAX_PULSE_LENGTH);
                      motC.writeMicroseconds(MAX_PULSE_LENGTH);
                      motD.writeMicroseconds(MAX_PULSE_LENGTH);
            break;
        }
    }
}

/**
 * Displays instructions to user
 */
void displayInstructions() {  
    Serial.println("READY - PLEASE SEND INSTRUCTIONS AS FOLLOWING :");
    Serial.println("\t1 : Send min throttle");
    Serial.println("\t2 : Send medium throttle");
    Serial.println("\t3 : Send max throttle\n");
}
