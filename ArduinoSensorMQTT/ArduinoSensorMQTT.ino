/*
    Arduino Data Collector

   Collects data and sends to an MQTT channel

   Author: Kevin Dunwell

    Version 1.1.0


     02/03/18  1.0.2   Changed SSID to VioletInternet for new hub
     02/03/18  1.0.3   Add support for DHT11. Add support for Photo-resistor
     02/03/18  1.0.4   Add WiFiEsp library and EEPROM storage for ssid/password
     03/01/18  1.0.5   Remove WiFiEsp in favour of home-grown ESP8266/ThingSpeak library
     18/3/18   1.1.0   Converted ESP8266 as MQTT communicator

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
    DynamicJsonBuffer jb;
    JsonObject& root = jb.parseObject(Serial1);

    if (root.success()) {
      Serial.println("Parse success");
      int ledset = root["state"]["desired"]["ledstate"];
      long delaymil = root["state"]["desired"]["delay"];
      if (delaymil > 1000) {
        Serial.print("New delay: ");
        Serial.print(delaymil);
        Serial.print("  LED:");
        Serial.println(ledset);

        bool changed = ((delayMillis != delaymil) || (lightOn != ledset));

        if (changed) {
          delayMillis = delaymil;
          lightOn = ledset;
          sendState();
        }
        setLED();
        syncInit = true;
      }
    } else {
      Serial.println("Parse fail");
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

}







