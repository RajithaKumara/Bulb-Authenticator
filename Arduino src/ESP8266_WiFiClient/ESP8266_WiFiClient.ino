#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

/**
 * EEPROM byte usage
 * 
 * 0: isFirstLoad
 * 1: isIdSet
 * 2: isPasswordSet
 * 3:
 * 4:
 * 5:
 * 6: bulbIdAddress
 * 7: password[0]
 * 8: password[1]
 * 9: password[2]
 * 10: password[3]
 * 11: password[4]
 * 12: password[5]
 * 13: password[6]
 * 14: password[7]
 */

uint8_t bulbID = 1;
bool  bulbState = false;

const char *ssid = "Main-AP";
char *password = "12345678";

const char *ssidAP = "Bulb-AP";
const char *passwordAP = "12345678";

const uint8_t isFirstLoadAddress = 0;
const uint8_t isIdSetAddress = 1;
const uint8_t isPasswordSetAddress = 2;
const uint8_t isQuickSwitchOnAddress = 3;
const uint8_t bulbIdAddress = 6;
const uint8_t passwordAddress = 7;
const int eeprom_length = 512;
const String url = "http://192.168.4.1/getBulbState?id=";

unsigned long _time;
int _timeout = 60000;
bool isTimeout = false;

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT); //relay output define
  bulbON();
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
  bulbOFF();
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  EEPROM.begin(eeprom_length);

  uint8_t isFirstLoad = EEPROM.read(isFirstLoadAddress);
  if (isFirstLoad == 1) { //not first loading
    uint8_t isIdSet = EEPROM.read(isIdSetAddress);
    uint8_t isPasswordSet = EEPROM.read(isPasswordSetAddress);
    uint8_t isQuickSwitchOn = EEPROM.read(isQuickSwitchOnAddress);
    if (isIdSet == 1) {
      bulbID = EEPROM.read(bulbIdAddress);
    }
    if (isPasswordSet == 1) {
      String strPass = "";
      for (int i = 0; i < 8; i++) {
        strPass += (char) EEPROM.read(passwordAddress + i);
      }
      strPass.toCharArray(password, 9);
    }
    if (isQuickSwitchOn) {
      bulbON();
    }

  } else { //if first _time
    _timeout = _timeout * 5;
    EEPROM.write(isFirstLoadAddress, 1);
    EEPROM.write(isIdSetAddress, 0);
    EEPROM.write(isPasswordSetAddress, 0);
    EEPROM.write(isQuickSwitchOnAddress, 0);
    EEPROM.commit();
  }


  //----------------------------------------------------------

  WiFi.softAP(ssidAP, passwordAP);
  server.on("/", handleRoot);
  server.on("/changeId", handleChangeId);
  server.on("/changePass", handleChangePassword);
  server.on("/quickSwitchOn", handleQuickSwitchOn);
  server.on("/finish", handleFinish);
  server.onNotFound(handleNotFound);
  server.begin();

  _time = millis();
  while (millis() - _time < _timeout) {
    server.handleClient();
  }

  delay(500);
  WiFi.softAPdisconnect();
  delay(500);
  WiFi.enableAP(false);
  delay(500);

  uint8_t isQuickSwitchOn = EEPROM.read(isQuickSwitchOnAddress);
  if (!isQuickSwitchOn) {
    for (int i = 0; i < 2; i++) {
      bulbON();
      digitalWrite(LED_BUILTIN, LOW);
      delay(2000);
      bulbOFF();
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
    }
  }

  //----------------------------------------------------------

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(1000);

  HTTPClient http;
  http.begin(url + bulbID);
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
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}



/**
   Functions
*/
void bulbON() {
  bulbState = true;
  //  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, HIGH); //relay on
}

void bulbOFF() {
  bulbState = false;
  //  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(2, LOW); //relay off
}

void changeId(uint8_t id) {
  EEPROM.write(bulbIdAddress, id);
  EEPROM.write(isIdSetAddress, 1); //id set
  EEPROM.commit();
  bulbID = id;
}

String changePassword(String pass) {
  for (int i = 0; i < 8; i++) {
    EEPROM.write(passwordAddress + i, pass[i]);
  }
  EEPROM.write(isPasswordSetAddress, 1); //password set
  EEPROM.commit();
  pass.toCharArray(password, 9);
  return password;
}

void quickSwitchOn(bool state) {
  EEPROM.write(isQuickSwitchOnAddress, state);
  EEPROM.commit();
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
  String pass = "12345678";
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

void handleQuickSwitchOn() {
  bool state = false;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "state") {
      if (server.arg ( i ) == "true") {
        state = true;
      }
    }
  }
  quickSwitchOn(state);
  server.send(200, "application/json", "{\"res\":\"OK\"}");
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

