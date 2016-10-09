#include <BLEAttribute.h>
#include <BLECentral.h>
#include <BLECharacteristic.h>
#include <BLECommon.h>
#include <BLEDescriptor.h>
#include <BLEPeripheral.h>
#include <BLEService.h>
#include <BLETypedCharacteristic.h>
#include <BLETypedCharacteristics.h>
#include <BLEUuid.h>
#include <CurieBLE.h>


// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central

BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService ledService ("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
BLEUnsignedCharCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

const int lowTemp = 25; // temp for theheater to kick on
const int highTemp = 28; // temp for the heater to kick off? or sustain?
const int tempSensorPin = A0; // define temp reading pin (Analog)
const int redLEDPin = A2;
const int greenLEDPin = A3;
float temperature;
const int B = 4275;               // B value of the thermistor


void setup() {
  Serial.begin(9600);

  // set LED pin to output mode; simulates heater
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("Car_Door_Heater");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchCharacteristic);

  // set the initial value for the characeristic:
  switchCharacteristic.setValue(0);

  // begin advertising BLE service:
  blePeripheral.begin();
}


void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // while the central is still connected to peripheral:
  while (central.connected()) {
    // if the remote device wrote to the characteristic,
    // use the value to control the LED:
    if (switchCharacteristic.written()) {
      if (switchCharacteristic.value()) {   // any value other than 0
        int temp = analogRead(tempSensorPin);

        float R = 1023.0 / ((float)temp) - 1.0;
        R = 100000.0 * R;
        temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;
        if (temperature <= lowTemp) {
          digitalWrite(redLEDPin, HIGH);
          digitalWrite(greenLEDPin, LOW);
          while (temperature < highTemp) {
            temp = analogRead(tempSensorPin);
            float R = 1023.0 / ((float)temp) - 1.0;
            R = 100000.0 * R;
            temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;

            Serial.print("temperature = ");
            Serial.println(temperature);
            Serial.println("Red LED should be on");

            delay(1000);
          }
        }
        else if (temperature > lowTemp) {
          digitalWrite(greenLEDPin, HIGH);
          digitalWrite(redLEDPin, LOW);

          while (temperature >= lowTemp) {
            temp = analogRead(tempSensorPin);
            float R = 1023.0 / ((float)temp) - 1.0;
            R = 100000.0 * R;
            temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15;
            Serial.print("temperature = ");
            Serial.println(temperature);
            Serial.println("Green LED should be on");

            delay(1000);
          }
        }
         digitalWrite(greenLEDPin, HIGH);
            digitalWrite(redLEDPin, LOW);  
      } 
    }
  }
}


