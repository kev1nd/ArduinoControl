/*
    Arduino Data Collector

   Collects data and sends to an MQTT channel

   Author: Kevin Dunwell

    Version 1.1.1


     02/03/18  1.0.2   Changed SSID to VioletInternet for new hub
     02/03/18  1.0.3   Add support for DHT11. Add support for Photo-resistor
     02/03/18  1.0.4   Add WiFiEsp library and EEPROM storage for ssid/password
     03/01/18  1.0.5   Remove WiFiEsp in favour of home-grown ESP8266/ThingSpeak library
     18/03/18  1.1.0   Converted ESP8266 as MQTT communicator
     05/04/18  1.1.1   Support for debug/information messages coming from the ESP8266

*/

#include <SPI.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>

#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

int pinDHT11 = 2;
SimpleDHT11 dht11;

int tempPin = 0;      // Sensor pin for Thermister
int lightPin = 1;     // Sensor pin for light level
int lightOn = 0;
int inputLevel = 2;
unsigned long lastMillis = 0;
unsigned long delayMillis = 10000;

boolean newData = false;
boolean syncInit = false;

void parseStream() {
  if (Serial1.available() > 0) {
    DynamicJsonBuffer jb(800);
    JsonObject& root = jb.parseObject(Serial1);

    if (root.success()) {
      Serial.println("Parse success");
      root.prettyPrintTo(Serial);
      Serial.println();
      if (root.containsKey("information")) {
        String msg = root["information"]; 
        Serial.print("ESP:"); 
        Serial.println(msg);
      } else  { //if (root.containsKey("state"))
        JsonObject& desired = root["state"]["desired"];
        //desired.prettyPrintTo(Serial);
        int ledset = root["state"]["desired"]["ledstate"];
        long delaymil = root["state"]["desired"]["delay"];
        bool changed = false;

        if (desired.containsKey("delay")) {
          Serial.print("New delay: ");
          Serial.println(delaymil);
          if (delayMillis != delaymil) {
            changed = true;
            delayMillis = delaymil;
          }
          syncInit = true;
        }

        if (desired.containsKey("ledstate")) {
          Serial.print("LED:");
          Serial.println(ledset);
          if (ledset != lightOn) {
            changed = true;
            lightOn = ledset;
            setLED();
          }
        }

        if (changed) {
          syncInit = true;
          sendState();
        }
      }


    }
  }
}

void setLED() {
  if (lightOn == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED off");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED on");
  }
}

void sendState() {
  Serial1.print("{ \"channel\": \"$aws/things/mosquittobroker/shadow/update\", \"payload\": ");
  Serial1.print("{\"state\":{\"reported\":");
  Serial1.print("{ \"ledstate\": ");
  Serial1.print(lightOn);
  Serial1.print(",\"delay\": ");
  Serial1.print(delayMillis);
  Serial1.println("}}}}");
}


float getThermister() {
  int tempReading = analogRead(tempPin);
  // This is OK
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  // float tempF = (tempC * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit
  return tempC;
}


void sendReadings() {
  int lightLevel  = analogRead(lightPin);
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.println("Read DHT11 failed");
    return;
  }
  float therm = getThermister();
  float volts = analogRead(inputLevel) / 18.65;

  Serial1.print("{ \"channel\": \"arduino/data\", \"payload\": ");
  Serial1.print("{\"light\": ");
  Serial1.print(lightLevel);
  Serial1.print(",\"temp\":");
  Serial1.print(temperature);
  Serial1.print(",\"humid\":");
  Serial1.print(humidity);
  Serial1.print(",\"therm\":");
  Serial1.print(therm);
  Serial1.print(",\"volts\":");
  Serial1.print(volts);
  Serial1.println("}}");

  Serial.println("Sent readings");
}


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);         // The port back to the PC, if connected
  Serial1.begin(9600);
  Serial.println("Ready");
  lastMillis = millis();
}

void loop()
{
  if (millis() - lastMillis > delayMillis) {
    lastMillis = millis();
    if (syncInit) {
      sendReadings();
    } else {
      Serial1.println("{ \"channel\": \"$aws/things/mosquittobroker/shadow/get\", \"payload\": }");
      Serial.println("Request config sent");
    }
  }
  parseStream();

//  while (Serial1.available()) {    
//    Serial.print((char)Serial1.read());
//  }

}








