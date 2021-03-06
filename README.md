# Arduino Control
An experimental project with an Arduino Uno linked to an ESP8266 ESP-01 Wifi module. 
It reads temperature, light levels and humidity, and sends the data to ThingSpeak.

The idea behind the project is learning and training, but it may become the basis of future IoT projects.

There are some good featues, that should be re-usable, for example.
1. Connect an Arduino to WiFi, and reliably send data using the WifiEsp library. (library also supports web service and more)
2. Interface to the ESP8266 ESP-01 module. This means a 3.3V voltage regulator for power (the Arduino 3.3V power can't provide enough current) and a TTL 5V to 3.3V step-down using two resistors.
3. Separate sketch loaded to ESP8266. This allowed the flexiility of Arduino Uno, and worked better than just using AT commands on the ESP8266 module.
4. Publish and subscribe to MQTT channels using ESP8266
5. Communicate with Alexa and AWS Internet of Things.
6. Lambda function that responds to an AWS IoT rule.
7. Lambda function that posts data to thingspeak.
8. Alexa skill that can update an AWS IoT "thing"


https://thingspeak.com/channels/418675

## How it works
The Arduino reads sensor data every "N" seconds (default 30s) and posts it as a data to the MQTT channel "arduino/data".
Configuration requests are received from the Thing Cloud, and can change the number of milliseconds between data postings. It can also switch the LED off and on.
Configuration changes are confirmed through another MQTT channel.
The Arduino acts as a device available to the Amazon Echo and can respond to commands.
A Wifi module is used to communicate with the network, sending and receiving commands using MQTT.
Communication between the Arduino and ESP8266 module us through the serial port. All communications are JSON based.

### Raspberry Pi
A Raspberry Pi is used as a local Hub, since the Arduino and/or ESP8266 is not capable of supporting TLS 1.2
Download and install the latest version of Mosquitto onto the Raspberry Pi.
There are several instructables to do this on line, but the trick to getting mine working was.
1. It was necessary to upgrade to the latest version manualls - the version that comes with a standard install wasn't good enough.
2. I created the certificates using the (excellent) AWS site, not the AWS command-line, then downloaded them to the Pi.
To do: add step-by-step instructions and working version numbers.

### Electronics
The electronics are the same as the project ArduinoSensor, and consist of:
1. An Arduino Uno, mounted next to a breadboard
2. An ESP8266 ESP-01, with standard AT Eprom
3. A 3.3V regulator, and step-down TTL voltage converter, to protect the ESP8266
4. A Thermister
5. A DHT11 temperature and humidity sensor
6. A light-sensitive resistor

![Picture of prototype board](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/pic1.jpg)

![Another picture of prototype board](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/pic2.jpg)

## Building your version
This is the wiring diagram for the project.
It's not complete yet (needs light-sensitice resistor circuit).

![Circuit Diagram](https://github.com/kev1nd/ArduinoSensor/blob/master/assets/circuitdiagram.jpg)

### Configure your ESP8266 for 9600 baud
The ESP8622 unit I bought was set to run at 115200 baud, which is too fast for software serial emulation on the Arduino to keep up with. However, sending this AT command to the unit will permanently configure it to 9600 baud:

`AT+UART_DEF=9600,8,1,0,0`

There are several articles around with sketches to send comms to the ESP8266 through Serial1. Since the Arduino will send, but not receive, quite happily at 115200, these can be used with this circuit. Alternatively, you can use an ESP8266 USB adapter talk to it directly from a PC.

### Change the ThingSpeak Key
Create yourself an account at https://thingspeak.com and create a channel. You will need four fields:-
1. Temperature (Thermister)
2. Temperature (DHT11)
3. Humidity (DHT11)
4. Light Level

## Folders
There several parts to this repository:
### Alexa
The Alexa skill, downloaded as a JSON file
### AlexaLambda
The Node.js Lambda function for the Alexa Skill
### ArduinoDataTrigger
The Node.js Lambda function that resonds to an IoT rule
### ArduinoSensorMQTT
The Arduino sketch
### mqtt_esp
The ESP8266 sketch



## Future
1. A web service to configure, rather than a serial port form
