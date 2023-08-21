/*
 * Code for BLUETOOTH COMMUNICATION inspired from BLE_uart example.
 * 
 * Create a BLE server that, once it received a connection, will send a periodic notifications.
 * The service advertises itself as: ...
 * Has a characteristic of: ... - used for receiving data with "WRITE"
 * Has a characteristic of: ... - used to send data with "NOTIFY"
 * 
 * The design of creating the BLE server is:
 * 1. Create a BLE Server.
 * 2. Create a BLE Service.
 * 3. Create a BLE Characteristic on the Service.
 * 4. Create a BLE Descriptor on the Characteristic.
 * 5. Start the Service.
 * 6. Start advertising.
 */

// Libraries for BLUETOOTH COMMUNICATION
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Libraries for SERIAL COMMUNICATION with Arduino Due
#include <HardwareSerial.h>

// Libraries for BLDC CONTROL
#include <ESP32Servo.h>
#include <ESC.h>          // RC_ESP library


// BLUETOOTH - Server Variables
BLEServer *pServer = NULL;
BLEService *pService;
BLECharacteristic *pTxCharacteristic;

// Device Name
#define DEVICE_NAME "ESP32 USV" // This name must be the same in the Bluetooth code and the web interface.

// UUIDs
#define SERVICE_UUID           "a4330cb6-680f-4843-89e1-221d3587b16b"   // UART service UUID
#define CHARACTERISTIC_UUID_RX "a4330cb6-680f-4843-89e1-221d3587b16c"
#define CHARACTERISTIC_UUID_TX "a4330cb6-680f-4843-89e1-221d3587b16d"

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;  // data to be sent to Arduino Due


// SERIAL - Pins
HardwareSerial SerialPort(2); // UART2 on ESP32
#define RXp2 16               // pin to receive data from Arduino Due
#define TXp2 17               // pin to send data to Arduino Due

// SERVO - Pins
#define SERVO_LEFT_PIN 12
#define SERVO_RIGHT_PIN 14
#define SERVO_MIDDLE_PIN 15

// BLDC - Pins
#define ESC_LEFT_PIN 27
#define ESC_RIGHT_PIN 26
#define ESC_MIDDLE_PIN 25


// BLDC - ESC Objects
ESC escLeft(ESC_LEFT_PIN,1000,2000,500);  // ESC_Name(PIN, Minimum Value, Maximum Value, Arm Value);
ESC escRight(ESC_RIGHT_PIN,1000,2000,500);
ESC escMiddle(ESC_MIDDLE_PIN,1000,2000,500);


// REAL TIME Parameters
unsigned long TimeT;
unsigned long Time0;
unsigned long K = 1;
unsigned long TCycle = 20;  // cycle time, in ms


// BLUETOOTH - Variables for Messages
String myS;
String Fx_string;
String Fy_string;
String Tz_string;
float Fx_value = 0;
float Fy_value = 0;
float Tz_value = 0;


// CONTROL ALLOCATION - Prototype Parameters


// CONTROL ALLOCATION - Variables
float thrustLeft;
float thrustRight;
float thrustMiddle;
float azimuthLeft;
float azimuthRight;
float azimuthMiddle;


// BLDC - Pulse Lengths
#define MIN_PULSE 1000  // minimum pulse length, in µs
#define MAX_PULSE 2000  // maximum pulse length, in µs



/*
 * 
 */
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer) {
      deviceConnected = false;
    }
};

/*
 * 
 */
String getValueS(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for ( int i=0; i<=maxIndex and found<=index; i++) {
        if ( data.charAt(i)==separator or i==maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i==maxIndex) ? i+1 : i;
        }
    }

    return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/*
 * 
 */
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        myS = "";
        
        if (rxValue.length() > 0) {
            //Serial.println("*********");
            //Serial.print("Received Value: ");
            for ( int i=0; i<rxValue.length(); i++ ) {
                //Serial.print(rxValue[i]);
                myS += rxValue[i];
            }
            //Serial.println();
            //Serial.println("*********");
        }
        
        Fx_string = getValueS(myS,',',0);
        Fy_string = getValueS(myS,',',1);
        Tz_string = getValueS(myS,',',2);
        
        Fx_value = (Fx_string).toInt();
        Fy_value = (Fy_string).toInt();
        Tz_value = (Tz_string).toInt();
    }
};

/*
 * 
 */
void setup() {
    Serial.begin(115200);                               // Serial Monitor
    SerialPort.begin(115200, SERIAL_8N1, RXp2, TXp2);   // Communication with Arduino

    pinMode(ESC_LEFT_PIN,OUTPUT);
    pinMode(ESC_RIGHT_PIN,OUTPUT);
    pinMode(ESC_MIDDLE_PIN,OUTPUT);

    pinMode(SERVO_LEFT_PIN,OUTPUT);
    pinMode(SERVO_RIGHT_PIN,OUTPUT);
    pinMode(SERVO_MIDDLE_PIN,OUTPUT);

    // Create the BLE Device
    BLEDevice::init(DEVICE_NAME);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                     );

    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                       );

    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
    delay(1000);
    
    Serial.println("Initialization of ESCs...");
    escLeft.arm();
    escRight.arm();
    escMiddle.arm();
    delay(5000);
}

/*
 * 
 */
void loop() {
    if (deviceConnected) {
        if ( K == 1 ) {
            Serial.println("Start");
            delay(3000);
            Time0 = millis();
        }

        // CONTROL ALLOATION


        // Commands


        // Serial Printing
        

        TimeT = millis() - Time0;

        if ( TimeT > K*TCycle ) {
            Serial.println("Sampling time too small...");
        }
        else {
            while ( millis() - Time0 < K*TCycle ) {
              // Waiting loop for real time application
            }
        }
        K++;
    }

    // disconnecting
    if (!deviceConnected and oldDeviceConnected) {
        delay(500); // Give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
        K = 1;
    }

    // connecting
    if (deviceConnected and !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
