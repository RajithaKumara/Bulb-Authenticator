#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

const char *ssid = "Default";
const char *password = "12345678";
struct Bulb {
  uint8_t id;
  int intensityOn;
  int intensityOff;
  bool state;
};
uint8_t bulbCount = 0;
Bulb bulbs[] = {};
int startAddress = 6; //This is start address of EEPROM
const int eeprom_length = 512;

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  Serial.begin(115200);
  EEPROM.begin(eeprom_length);
  //  byte len = EEPROM.length();

  uint8_t isFirstLoad = EEPROM.read(0);
  if (isFirstLoad == 1) { //not first loading
    bulbCount = EEPROM.read(1); //load bulb data from ROM
  } else { //if first time
    EEPROM.write(0, 1);
    EEPROM.commit();
    Serial.println("firsttime");
  }

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/addBulb", handleAddBulb);
  server.on("/removeBulb", handleRemoveBulb);
  server.on("/changeBulbState", handleChangeBulbState);
  server.on("/getBulbState", handleGetBulbState);//wifi client
  server.on("/setIntensity", handleSetIntensity);
  server.on("/getDetails", handleGetDetails);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}


/**
   Functions
*/
void addBulb(uint8_t id) {
  Bulb bulb = {id, 0, 0, false};
  saveToRom(bulb);
  bulbCount += 1;
}

void addBulb(uint8_t id, int intensityOn, int intensityOff, bool state) {
  Bulb bulb = {id, intensityOn, intensityOn, state};
  saveToRom(bulb);
  bulbCount += 1;
}

void removeBulb(uint8_t bulbID) {
  Bulb newBulbs[] = {};
  uint8_t count = 0;
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id != bulbID) {
      newBulbs[count] = bulb;
      count += 1;
    }
  }
  count += 1; //pointer move to next
  for (int j = 0; j < count; j++) {
    bulbs[j] = newBulbs[j];
  }
  bulbCount = count;
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
      byte tempIntensityOn = EEPROM_read_2Byte(bulbAddress + 2);
      byte tempIntensityOff = EEPROM_read_2Byte(bulbAddress + 4);
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
    }else{
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
  EEPROM.write(1, bulbCount);
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
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "application/json", "{\"res\":\"OK\"}");
  digitalWrite(LED_BUILTIN, LOW);
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
  bool state = getBulbState(id.toInt());
  String res = "{\"res\":";
  res += state;
  res += "}";

  server.send ( 200, "application/json", res );
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

void handleGetDetails() {
  String res = "{\"res\":";
  res += getAllDetails();
  res += "}";

  server.send ( 200, "application/json", res );
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
