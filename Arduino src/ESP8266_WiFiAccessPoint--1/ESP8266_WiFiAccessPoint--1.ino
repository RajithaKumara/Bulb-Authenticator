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

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  Serial.begin(115200);
  //  EEPROM.begin(512);
  //  byte len = EEPROM.length();
  //  getFromRom(); //load bulb data from ROM
  bulbCount = EEPROM.read(0);

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
  delay(1);
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
    }
  }
}

void setBulbIntensity(uint8_t bulbID, int intensity, bool isOn) {
  for (int i = 0; i < bulbCount; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);

    if (tempId == bulbID) {
      if (isOn) {
        EEPROM.write(bulbAddress + 2, intensity);
        EEPROM.write(bulbAddress + 3, intensity);
      } else {
        EEPROM.write(bulbAddress + 4, intensity);
        EEPROM.write(bulbAddress + 5, intensity);
      }
    }
  }
  //  saveToRom(); //save to rom
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
      byte tempIntensityOn = EEPROM.read(bulbAddress + 2);
      byte tempIntensityOn2 = EEPROM.read(bulbAddress + 3);
      byte tempIntensityOff = EEPROM.read(bulbAddress + 4);
      byte tempIntensityOff2 = EEPROM.read(bulbAddress + 5);
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

    json += "{\"id\":\"";
    json += EEPROM.read(bulbAddress);
    json += "\",\"intensityOn\":\"";
    json += EEPROM.read(bulbAddress + 2);
    json += "\",\"intensityOff\":\"";
    json += EEPROM.read(bulbAddress + 4);
    json += "\",\"state\":\"";
    json += EEPROM.read(bulbAddress + 1);
    json += "\"}";
  }
  json += "]";
  return json;
}

void saveToRom(Bulb bulb) {
  EEPROM.write(0, bulbCount);

  int bulbAddress = startAddress + bulbCount * 6;
  EEPROM.write(bulbAddress, bulb.id); //Bulb block size  = 6 byte
  EEPROM.write(bulbAddress + 1, bulb.state);
  EEPROM.write(bulbAddress + 2, bulb.intensityOn);
  EEPROM.write(bulbAddress + 3, bulb.intensityOn);
  EEPROM.write(bulbAddress + 4, bulb.intensityOff);
  EEPROM.write(bulbAddress + 5, bulb.intensityOff);
}

void getFromRom() {
  int numOfBulbs = EEPROM.read(0);
  if (numOfBulbs == 255) {
    numOfBulbs = 0;
  }

  for (int i = 0; i < numOfBulbs; i++) {
    int bulbAddress = startAddress + i * 6;
    int tempId = EEPROM.read(bulbAddress);
    byte tempState = EEPROM.read(bulbAddress + 1);
    byte tempIntensityOn = EEPROM.read(bulbAddress + 2);
    byte tempIntensityOn2 = EEPROM.read(bulbAddress + 3);
    byte tempIntensityOff = EEPROM.read(bulbAddress + 4);
    byte tempIntensityOff2 = EEPROM.read(bulbAddress + 5);
    Serial.print("bulb " + i);
    Serial.println("==> tempId" + tempId);


    addBulb(tempId, tempIntensityOn, tempIntensityOff, tempState);
  }
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

  delay(1);
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

  delay(1);
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

  delay(1);
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
  String res = "{\"res\":\"";
  res += state;
  res += "\"}";

  delay(1);
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

  delay(1);
  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleGetDetails() {
  String res = getAllDetails();

  delay(1);
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
