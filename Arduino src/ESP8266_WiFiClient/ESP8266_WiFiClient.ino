#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WiFiMulti WiFiMulti;

uint8_t bulbID = 0;
bool  bulbState = false;

const char *ssid = "Main-AP";
char *password = "12345678";

const char *ssidAP = "Bulb-AP";
const char *passwordAP = "12345678";

String url = "http://192.168.4.1/getBulbState?id=";
const int eeprom_length = 512;
unsigned long _time;
int _timeout = 60000;
const uint8_t bulbIdAddress = 6;
const uint8_t passwordAddress = 7;

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //relay output define
  delay(1000);
  Serial.begin(115200);
  EEPROM.begin(eeprom_length);

  uint8_t isFirstLoad = EEPROM.read(0);
  if (isFirstLoad == 1) { //not first loading
    uint8_t isIdSet = EEPROM.read(1);
    uint8_t isPasswordSet = EEPROM.read(2);
    if (isIdSet == 1) {
      bulbID = EEPROM.read(bulbIdAddress);
    }
    if (isPasswordSet == 1) {
      char passArray[8] = "";
      String strPass = "";
      for (int i = 0; i < 8; i++) {
        passArray[i] = EEPROM.read(passwordAddress + i);
        strPass = passArray[i];
      }
      strPass.toCharArray(password, 9);
    }
  } else { //if first _time
    _timeout = _timeout * 5;
    EEPROM.write(0, 1);
    EEPROM.commit();
  }

  //--------------------------------------------------

  WiFi.softAP(ssidAP, passwordAP);

  server.on("/", handleRoot);
  server.on("/changeId", handleChangeId);
  server.on("/changePass", handleChangePassword);
  server.on("/finish", handleFinish);
  server.onNotFound(handleNotFound);
  server.begin();

  _time = millis();
  while (millis() - _time < _timeout) {
    server.handleClient();
  }
  delay(500);

  WiFi.softAPdisconnect(true); // Stop the AP
  delay(500);
  //--------------------------------------------------

  WiFiMulti.addAP(ssid, password); // Start WI-Fi client
  delay(500);

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


/**
   Functions
*/
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

void changeId(uint8_t id) {
  EEPROM.write(bulbIdAddress, id);
  EEPROM.commit();
  bulbID = id;
}

String changePassword(String pass) {
  for (int i = 0; i < 8; i++) {
    EEPROM.write(passwordAddress + i, pass[i]);
  }
  EEPROM.commit();
  pass.toCharArray(password, 9);
  return password;
}


/**
   Handle Routes
*/
void handleRoot() {
  digitalWrite(LED_BUILTIN, HIGH);
  _time = millis();
  server.send(200, "application/json", "{\"res\":\"OK\"}");
  digitalWrite(LED_BUILTIN, LOW);
}

void handleChangeId() {
  _time = millis();
  String id;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    }
  }
  changeId(id.toInt());
  server.send(200, "application/json", "{\"res\":\"OK\"}");
}

void handleChangePassword() {
  _time = millis();
  String pass;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "password") {
      pass = server.arg ( i );
    }
  }
  String res = "{\"res\":\"";
  res += changePassword(pass);
  res += "\"}";

  server.send(200, "application/json", res);
}

void handleFinish() {
  _timeout = 10;
  server.send(200, "application/json", "{\"res\":\"OK\"}");
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, HIGH);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite(LED_BUILTIN, LOW);
}

