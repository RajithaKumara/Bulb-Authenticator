#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

uint8_t bulbID = 0;
bool  bulbState = false;
const char *ssid = "Default";
const char *password = "12345678";
String url = "http://192.168.4.1/getBulbState?id=" + bulbID;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //relay output define
  delay(1000);
  Serial.begin(115200);

  WiFiMulti.addAP(ssid, password);

}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String state = http.getString();

        if (state == "ON" && bulbState == false) {
          bulbON();
        } else if (state == "OFF" && bulbState == true) {
          bulbOFF();
        }
      }
    } else {
//      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  
  delay(1000);
}

void bulbON() {
  bulbState = true;
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(2, HIGH); //relay on
}

void bulbOFF() {
  bulbState = false;
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, LOW); //relay off
}

