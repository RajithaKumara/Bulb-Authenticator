# Bulb Authenticator

> This project contains three components Mobile app, Arduino main unit and ESP8266 wifi client. Basically main unit track light intensity level and broadcast to bulbs via Wi-Fi. HTTP used as communication protocol. In the main controller Arduino board and ESP8266 Wi-Fi module communicate via serial communication. Mobile app used to introduce new Wi-Fi client(Bulb adapter) to the main controller. All the microcontroller data saved in the EEPROM.

## Prerequisites
* Ionic-cli  ([getting-started](https://ionicframework.com/getting-started/#cli))
* Cordova ([getting-started](https://cordova.apache.org/docs/en/latest/guide/cli/))
* Arduino IDE ([download page](https://www.arduino.cc/en/Main/Software))

## Usage
First clone the repository.
``` bash
$ git clone https://github.com/RajithaKumara/Bulb-Authenticator
```
Go to inside directory.
``` bash
$ cd Bulb-Authenticator
```
Install dependencies.
``` bash
$ npm install
```
Serve with live reload at localhost:8100 (default).
``` bash
$ Ionic serve
```
Add the platforms that you want to target your app. It will also add the required Cordova plugins to the project directory.
``` bash
$ cordova platform add android
$ cordova platform add ios
```

## Arduino Setup

* [ESP8266 WiFi Access Point](https://github.com/RajithaKumara/Bulb-Authenticator/tree/master/Arduino%20src/ESP8266_WiFiAccessPoint--1)
* [ESP8266 WiFi Client](https://github.com/RajithaKumara/Bulb-Authenticator/tree/master/Arduino%20src/ESP8266_WiFiClient)
* [Arduino Light Intensity Tracker](https://github.com/RajithaKumara/Bulb-Authenticator/blob/master/Arduino%20src/sensor_broadcast.ino)