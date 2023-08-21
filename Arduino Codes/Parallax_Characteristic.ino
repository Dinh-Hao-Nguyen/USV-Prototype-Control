#include <Servo.h>


// Servomotor CONTROL Signals
Servo servo1;         // corresponds to LEFT rear servo
Servo servo2;         // corresponds to RIGHT rear servo
Servo servo3;         // corresponds to MIDDLE front servo
#define PIN_SERVO1 5
#define PIN_SERVO2 6
#define PIN_SERVO3 7

// Servomotor FEEDBACK Signals
#define PIN_FEEDBACK1 2
#define PIN_FEEDBACK2 3
#define PIN_FEEDBACK3 4


// Parameters of Parallax Feedback 360° High Speed Servo
#define units_full_circle 360           // in degrees
#define q2min (units_full_circle / 4)   // Quadrant 2 minimum, in degrees (q2min = 90°)
#define q3max (q2min * 3)               // Quadrant 3 maximum, in degrees (q3max = 270°)
float dc_min = 2.9;                     // Minimum duty cycle of feedback signal, in %
float dc_max = 97.1;                    // Maximum duty cycle of feedback signal, in %


// Real time parameters
float current_time;
float previous_time;
float time0;
#define t_cycle (10*1000)           // Sampling time, in µs (Ts = 10ms)
#define t_exp (40*1000*1000)       // Duration of experiment, in µs (Texp = 200s)
#define nb_cycles (t_exp/t_cycle)   // Number of cycles during experiment
#define t_start (5*1000*1000)       // Time of start of experiment, in µs (Tstart = 5s)
#define t_end (70*1000*1000)       // Time of end of experiment, in µs (Tend = 210s) - includes buff time to display data


// Data
float dutyCycle;          // Feedback signal's duty cycle, in %
float pos; float oldPos;  // Motor's angular position, in degrees - measured by feedback sensor
float vel;                // Motor's rotation speed, in degrees/s
float cmd = 0;            // Command sent to motor
int k;

// STORED DATA
float time_save[2000];            // Vector to save time, in s
float t_cycle_save[2000];         // Vector to save cycle time, in s
float pos_save[2000];             // Vector to save position, in degrees
float vel_save[2000];             // Vector to save velocity, in degrees/s
float cmd_save[2000];             // Vector to save command

/*
 * Initialization
 */
void setup() {
  Serial.begin(115200);       // set serial communication rate, in bauds

  analogWriteResolution(12);  // set resolution of analog outputs (2^12 = 4096)
  analogReadResolution(12);   // set resolution of analog inputs (2^12 = 4096)

  pinMode(PIN_SERVO1,OUTPUT);
  pinMode(PIN_SERVO2,OUTPUT);
  pinMode(PIN_SERVO3,OUTPUT);
  pinMode(PIN_FEEDBACK1,INPUT);
  pinMode(PIN_FEEDBACK2,INPUT);
  pinMode(PIN_FEEDBACK3,INPUT);

  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo3.attach(PIN_SERVO3);

  Serial.println("Waiting phase...");
}


void loop() {
  while( micros() < t_start ) {   // wait to start experiment
  }

  while ( micros() >= t_start and micros() < t_end ) {
    Serial.println("Start experiment...");
    time0 = micros();                               // initialize time
    previous_time = time0;

    for ( int j=0; j<nb_cycles; j++ ) {
      current_time = micros();
      dutyCycle = getPWM(PIN_FEEDBACK1);            // read feedback signal's duty cycle
//      Serial.print("duty cycle: "); Serial.println(dutyCycle);
      pos = dc2pos(dutyCycle);                      // convert it into motor's angular position
      vel = (pos - oldPos) / t_cycle * 1000*1000;   // calculate velocity from position - t_cycle in µs converted into seconds
//      Serial.print("angle: "); Serial.print(pos-oldPos); Serial.print("\t"); Serial.print(t_cycle/1000/1000); Serial.print("\t"); Serial.println(vel);
      oldPos = pos;

//      if ( j%500 == 0 ) {   // increase command each 500*Ts
//        cmd = cmd + 10;
//      }
      if ( current_time - time0 < t_start + 2*1000*1000 ) {
          cmd = 1550;
      }
      else {
          cmd = 1650;
      }
      servo1.writeMicroseconds(cmd);
//      analogWrite(PIN_SERVO1,cmd);

      if ( j%10 == 0) {     // save data each 10*Ts
        k = j/10;
        cmd_save[k] = cmd;
        time_save[k] = (current_time - time0)/1000/1000;
        pos_save[k] = pos;
        vel_save[k] = vel;
        t_cycle_save[k] = (current_time - previous_time)/1000/1000;

//        Serial.print("P: "); Serial.print(pos_save[k],3);
//        Serial.print(", \t V: "); Serial.print(vel_save[k],3);
//        Serial.print(", \t C: "); Serial.print(cmd_save[k]);
//        Serial.print(", \t T: "); Serial.print(time_save[k],3);
//        Serial.print(", \t Tcycle: "); Serial.println(t_cycle_save[k],3);
      }

      previous_time = current_time;

      while ( micros() - time0 < (j+1)*t_cycle ) {
      }

    }

    // Display all the data at the end of experiment
    Serial.println("Display of the stored data...");
    for ( int i=0; i<nb_cycles/10; i++) {
      Serial.print(pos_save[i],3);
      Serial.print(", \t"); Serial.print(vel_save[i],3);
      Serial.print(", \t"); Serial.print(cmd_save[i]);
      Serial.print(", \t"); Serial.print(time_save[i],3);
      Serial.print(", \t"); Serial.println(t_cycle_save[i],3);
    }
  }

}


/*
 * Calculate the DUTY CYCLE of a PWM signal received by a pin.
 */
float getPWM(byte pin) {
  unsigned long highPulse = pulseIn(pin,HIGH,50000UL);  // measure high pulse, 50 ms timeout (try: 1.1 ms)
  unsigned long lowPulse = pulseIn(pin,LOW,50000UL);    // measure low pulse, 50 ms timeout

  // Check cycle time
//  if ( highPulse+lowPulse < 1000 or highPulse+lowPulse > 1200 ) {
//    Serial.println("INVALID CYCLE TIME");
//  }

  // pulseIn() returns zero on timeout
  if ( highPulse == 0 or lowPulse == 0 ) {
    return digitalRead(pin) ? 100 : 0;  // HIGH -> 100%, LOW -> 0%
  }

  return (100 * highPulse) / (highPulse + lowPulse);  // duty cycle (in %)
}

/*
 * Convert feedback signal's duty cycle into motor's ANGULAR POSITION.
 */
float dc2pos(float dc) {
  float angle = (dc - dc_min) * units_full_circle / (dc_max - dc_min + 1);

  if ( angle < 0) {
    angle = 0;
  }
  else if ( angle > units_full_circle - 1 ) {
    angle = units_full_circle - 1;
  }

  return angle;
}
