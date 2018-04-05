/*
    MQTT for ESP8266 ESP-01 module

    Communicates with the MQTT broker running on Raspberry Pi over Wifi,
    and the Arduino over serial port.

    Author: Kevin Dunwell

    Version:
    1.1  Update to use AWS standard IoT MQTT channels

*/

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "ssid.h"

#ifndef ssid_h
#define ssid_h
const char ssid[] = "ssid";
const char pass[] = "password";
#endif

WiFiClient net;
MQTTClient client(512);

unsigned long lastMillis = 0;

void logData(String jsonStr) {
  Serial.println("{\"information\":" + jsonStr + "}");
}

void logMessage(String msg) {
  logData("\"" + msg + "\"");
}

void connect() {
  logMessage("Checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(1000);
  }
  logMessage("Connected to Wifi. Trying MQTT");
  while (!client.connect("arduino", "try", "try")) {
    // Serial.print(".");
    delay(1000);
  }

  logMessage("Connected to MQTT");
  
  client.subscribe("$aws/things/mosquittobroker/shadow/+/accepted");
  // client.subscribe("$aws/things/mosquittobroker/shadow/update/accepted");
  // client.unsubscribe("arduino/config");
}

void messageReceived(String &topic, String &payload) {
  logMessage(topic);
  payload.replace("\n","");
  payload.replace("\r","");
  // Serial.println("{\"channel\":\"" + topic + "\",\"payload\":" + payload + "}");
  Serial.println(payload);
}

void publishData() {
  String inMsg = "";
  while (Serial.available()) {
    inMsg += Serial.readString();
    delay(100);
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(inMsg);

  String mqttSubscri = root["subscribe"];
  if (mqttSubscri.length() > 0) {
    client.subscribe(mqttSubscri);
    //logMessage("Subscribed to: " + mqttSubscri);
  };

  String mqttChannel = root["channel"];
  String mqttPayload = root["payload"];
  if (mqttChannel.length() > 0) {
    // logMessage("Publish: [" + mqttChannel + "] " + mqttPayload);
    client.publish(mqttChannel, mqttPayload);
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  client.begin("pi", net);
  client.onMessage(messageReceived);
  delay(1000);
  connect();
}

void loop() {
  client.loop();
  delay(10);
  
  if (!client.connected()) {
    connect();
  }
  if (Serial.available()) {
    publishData();
  }

}


