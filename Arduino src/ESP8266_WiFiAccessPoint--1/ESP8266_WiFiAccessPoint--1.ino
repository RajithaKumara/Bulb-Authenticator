#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/**
 * EEPROM byte usage
 * 
 * 0: isFirstLoad
 * 1: bulbCount
 * 2:
 * 3:
 * 4: password[0]
 * 5: password[1]
 * 6: password[2]
 * 7: password[3]
 * 8: password[4]
 * 9: password[5]
 * 10: password[6]
 * 11: password[7]
 * 12: bulb1.id
 * 13: bulb1.state
 * 14: bulb1.intensityOn MSByte (Most Significant Byte)
 * 15: bulb1.intensityOn LSByte (Least Significant Byte)
 * 16: bulb1.intensityOff MSByte
 * 17: bulb1.intensityOff LSByte
 * 18: bulb2................ 6 Bytes for a bulb
 */

const char *ssid = "Main-AP";
char *password = "12345678";

const uint8_t isFirstLoadAddress = 0;
const uint8_t bulbCountAddress = 1;
const uint8_t isPasswordSetAddress = 2;
const uint8_t passwordAddress = 4;
const uint8_t startAddress = 12; //This is start address of EEPROM for store bulb details
const int eeprom_length = 512;
const uint8_t timeout = 2;
const uint8_t payloadLength = 7;

struct Bulb {
  uint8_t id;
  int intensityOn;
  int intensityOff;
  bool state;
};
uint8_t bulbCount = 0;
unsigned long _time;
int current_intensity = 0;
int current_state = true; //true ==> up, false ==> down

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  EEPROM.begin(eeprom_length);

  uint8_t isFirstLoad = EEPROM.read(isFirstLoadAddress);
  if (isFirstLoad == 1) { //not first loading
    bulbCount = EEPROM.read(bulbCountAddress); //load bulb data from ROM
    uint8_t isPasswordSet = EEPROM.read(isPasswordSetAddress);
    if (isPasswordSet == 1) {
      String strPass = "";
      for (int i = 0; i < 8; i++) {
        strPass += (char) EEPROM.read(passwordAddress + i);
      }
      strPass.toCharArray(password, 9);
    }
  } else { //if first time
    EEPROM.write(isFirstLoadAddress, 1);
    EEPROM.write(bulbCountAddress, 0); // set bulb count to zero
    EEPROM.commit();
  }

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/addBulb", handleAddBulb);
  server.on("/removeBulb", handleRemoveBulb);
  server.on("/changeBulbState", handleChangeBulbState);
  server.on("/getBulbState", handleGetBulbState);//wifi client
  server.on("/setIntensity", handleSetIntensity);
  server.on("/getIntensity", handleGetIntensity);
  server.on("/getDetails", handleGetDetails);
  server.on("/changePassword", handleChangePassword);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();

  // read intensity value from main arduino
  char nextChar = Serial.read();
  if (nextChar == 'a') {
    String current_intensity_tmp = "";
    int i = 0;
    _time = millis();
    while (i < payloadLength && (millis() - _time) < timeout) {
      nextChar = Serial.read();
      if (isDigit(nextChar)) {
        current_intensity_tmp += nextChar;
        i++;
      } else {
        delay(1);
      }
    }
    if (current_intensity_tmp.length() == payloadLength) {
      current_intensity = current_intensity_tmp.substring(0, 5).toInt();
      current_state = current_intensity_tmp.substring(5).toInt();
      controlBulbState();
    }
  }
}


/**
   Functions
*/
void addBulb(uint8_t id) {
  Bulb bulb = {id, 0, 0, false};
  saveToRom(bulb);
}

void addBulb(uint8_t id, int intensityOn, int intensityOff, bool state) {
  Bulb bulb = {id, intensityOn, intensityOn, state};
  saveToRom(bulb);
}

void removeBulb(uint8_t bulbID) {
  bool isRemoved = false;
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);
    if (isRemoved) {
      EEPROM_write_2Byte(bulbAddress + 2 - 6, EEPROM_read_2Byte(bulbAddress + 2));
      EEPROM_write_2Byte(bulbAddress + 4 - 6, EEPROM_read_2Byte(bulbAddress + 4));
      EEPROM.write(bulbAddress - 6, tempId);
      EEPROM.write(bulbAddress + 1 - 6, EEPROM.read(bulbAddress + 1));
      EEPROM.commit();
    } else if (tempId == bulbID) {
      isRemoved = true;
    }
  }
  if (isRemoved) {
    bulbCount -= 1;
    EEPROM.write(bulbCountAddress, bulbCount);
    EEPROM.commit();
  }
}

void changeBulbState(uint8_t bulbID, bool state) {
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);

    if (tempId == bulbID) {
      EEPROM.write(bulbAddress + 1, state);
      EEPROM.commit();
    }
  }
}

void setBulbIntensity(uint8_t bulbID, int intensity, bool isOn) {
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);

    if (tempId == bulbID) {
      if (isOn) {
        EEPROM_write_2Byte(bulbAddress + 2, intensity);
      } else {
        EEPROM_write_2Byte(bulbAddress + 4, intensity);
      }
    }
  }
}

bool getBulbState(uint8_t bulbID) {
  Bulb bulb = findById(bulbID);
  return bulb.state;
}

Bulb findById(uint8_t bulbID) {
  Bulb bulb = {0, 0, 0, 0};
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);
    if (tempId == bulbID) {
      byte tempState = EEPROM.read(bulbAddress + 1);
      int tempIntensityOn = EEPROM_read_2Byte(bulbAddress + 2);
      int tempIntensityOff = EEPROM_read_2Byte(bulbAddress + 4);
      bulb = {tempId, tempIntensityOn, tempIntensityOff, tempState};
      return bulb;
    }
  }
  return bulb;
}

String getAllDetails() {
  String json = "[";
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    json += "{\"id\":";
    json += (String) EEPROM.read(bulbAddress);
    json += ",\"intensityOn\":";
    json += (String) EEPROM_read_2Byte(bulbAddress + 2);
    json += ",\"intensityOff\":";
    json += (String) EEPROM_read_2Byte(bulbAddress + 4);
    json += ",\"state\":";
    json += (String) EEPROM.read(bulbAddress + 1);
    if (i == bulbCount - 1) {
      json += "}";
    } else {
      json += "},";
    }
  }
  json += "]";
  return json;
}

void saveToRom(Bulb bulb) {
  int bulbAddress = startAddress + bulbCount * 6;
  EEPROM_write_2Byte(bulbAddress + 2, bulb.intensityOn);
  EEPROM_write_2Byte(bulbAddress + 4, bulb.intensityOff);

  EEPROM.write(bulbAddress, bulb.id); //Bulb block size  = 6 byte
  EEPROM.write(bulbAddress + 1, bulb.state);
  bulbCount += 1;
  EEPROM.write(bulbCountAddress, bulbCount);
  EEPROM.commit();
}

void EEPROM_write_2Byte(int address, int value) {
  byte leastSignificant8Bit = value;
  byte mostSignificant8Bit = value >> 8;
  EEPROM.write(address, mostSignificant8Bit);
  EEPROM.write(address + 1, leastSignificant8Bit);
  EEPROM.commit();
}

int EEPROM_read_2Byte(int address) {
  byte mostSignificant8Bit = EEPROM.read(address);
  byte leastSignificant8Bit = EEPROM.read(address + 1);
  int value = (mostSignificant8Bit << 8) + leastSignificant8Bit;
  return value;
}

String changePassword(String pass, String oldPass) {
  if (!oldPass.equals(password)) {
    return "ERROR";
  }
  for (int i = 0; i < 8; i++) {
    EEPROM.write(passwordAddress + i, pass[i]);
  }
  EEPROM.write(isPasswordSetAddress, 1); //password set
  EEPROM.commit();
  pass.toCharArray(password, 9);
  return "OK";
}

void controlBulbState(){
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempIntensityOn = EEPROM_read_2Byte(bulbAddress + 2);
    int tempIntensityOff = EEPROM_read_2Byte(bulbAddress + 4);
    byte tempState = EEPROM.read(bulbAddress + 1);
    if (tempIntensityOn < current_intensity && !current_state && tempState == 0) {
      EEPROM.write(bulbAddress + 1, true);
    }else if (tempIntensityOff > current_intensity && current_state && tempState == 1) {
      EEPROM.write(bulbAddress + 1, false);
    }
    EEPROM.commit();
  }
}

void printEEPROM() {
  String reading = "";
  Serial.println("----------------------------------------------------------");
  for (int i = 0; i < eeprom_length; i++) {
    reading += EEPROM.read(i);
    reading += " | ";
    if (i % 50 == 0) {
      Serial.println(reading);
      reading = "";
    }
  }
  Serial.println("----------------------------------------------------------");
}


/**
   Handle Routes
*/
void handleRoot() {
  server.send(200, "application/json", "{\"res\":\"OK\"}");
}

void handleAddBulb() {
  String id;
  String intensityOn;
  String intensityOff;
  uint8_t count = 0;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
      count += 1;
    } else if (server.argName ( i ) == "intensityOn") {
      intensityOn = server.arg ( i );
      count += 1;
    } else if (server.argName ( i ) == "intensityOff") {
      intensityOff = server.arg ( i );
      count += 1;
    }
  }

  if (count == 1) {
    addBulb(id.toInt());
  } else if (count == 3) {
    addBulb(id.toInt(), intensityOn.toInt(), intensityOff.toInt(), false);
  }

  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleRemoveBulb() {
  String id;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    }
  }
  removeBulb(id.toInt());

  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleChangeBulbState() {
  String id;
  bool state = false;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    } else if (server.argName ( i ) == "state") {
      if (server.arg ( i ) == "true") {
        state = true;
      }
    }
  }
  changeBulbState(id.toInt(), state);

  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleGetBulbState() {
  String id;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    }
  }
  String res = "OFF";
  if (getBulbState(id.toInt())){
    res = "ON";
  }

  server.send ( 200, "text/plain", res );
}

void handleSetIntensity() {
  String id;
  String intensity;
  bool isOn = false;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    } else if (server.argName ( i ) == "intensity") {
      intensity = server.arg ( i );
    } else if (server.argName ( i ) == "isOn") {
      if (server.arg ( i ) == "true") {
        isOn = true;
      }
    }
  }
  setBulbIntensity(id.toInt(), intensity.toInt(), isOn);

  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleGetIntensity() {
  String res = "{\"res\":\"";
  res += current_intensity;
  res += "\",\"state\":\"";
  res += current_state;
  res += "\"}";

  server.send ( 200, "application/json", res );
}

void handleGetDetails() {
  String res = "{\"res\":";
  res += getAllDetails();
  res += "}";

  server.send ( 200, "application/json", res );
}

void handleChangePassword() {
  String pass;
  String oldPass;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "password") {
      pass = server.arg ( i );
    } else if (server.argName ( i ) == "oldPassword") {
      oldPass = server.arg ( i );
    }
  }
  String result = changePassword(pass, oldPass);
  String res = "{\"res\":\"";
  res += result;
  res += "\"}";

  server.send(200, "application/json", res);
  if (result.equals("OK")) {
    WiFi.softAP(ssid, password); //change the current password
  }
}


void handleNotFound() {
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
}
