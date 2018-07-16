#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "Default";
const char *password = "12345678";

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  Serial.begin(115200);

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

/**
   addBulb(uint8_t id)
   Code 1111
*/
void addBulb(uint8_t id) {
  String payload = "ssss1111,";
  payload += id;
  handleSerial(payload);
}

/**
   addBulb(uint8_t id, int intensityOn, int intensityOff)
   Code 2222
*/
void addBulb(uint8_t id, int intensityOn, int intensityOff) {
  String payload = "ssss2222,";
  payload += id;
  payload += ",";
  payload += intensityOn;
  payload += ",";
  payload += intensityOff;
  handleSerial(payload);
}

/**
   removeBulb(uint8_t bulbID)
   Code 3333
*/
void removeBulb(uint8_t id) {
  String payload = "ssss3333,";
  payload += id;
  handleSerial(payload);
}

/**
   changeBulbState(uint8_t bulbID, bool state)
   Code 4444
*/
void changeBulbState(uint8_t id, bool state) {
  String payload = "ssss4444,";
  payload += id;
  payload += ",";
  payload += state;
  handleSerial(payload);
}

/**
   getBulbState(uint8_t bulbID)
   Code 5555
*/
bool getBulbState(uint8_t id) {
  String payload = "ssss5555,";
  payload += id;
  handleSerial(payload);
}

/**
   getAllDetails()
   Code 6666
*/
String getAllDetails() {
  String payload = "ssss6666";
  handleSerial(payload);
}

/**
   setBulbIntensity(uint8_t bulbID, int intensity, bool onOff)
   Code 7777
*/
void setBulbIntensity(uint8_t id, int intensity, bool onOff) {
  String payload = "ssss7777,";
  payload += id;
  payload += ",";
  payload += intensity;
  payload += ",";
  payload += onOff;
  handleSerial(payload);
}

Bulb findById(uint8_t bulbID) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      return bulb;
    }
  }
}

/**
   Send 'a' until get responce 'b'
*/
void handleSerial(String payload) {
  while (true){
    Serial.print('a');
    char responceChar = Serial.read();
    if (responceChar == 'b') {
      Serial.println(payload);
      Serial.println("\n\n");
      break;
    }else{
      delay(1);
    }
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
  } else if (count == 2) {
    addBulb(id.toInt(), intensityOn.toInt(), intensityOff.toInt());
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
  String res = "{\"res\":\"";
  res += state;
  res += "\"}";

  server.send ( 200, "application/json", res );
}

void handleSetIntensity() {
  String id;
  String intensity;
  bool onOff = false;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    } else if (server.argName ( i ) == "intensity") {
      intensity = server.arg ( i );
    } else if (server.argName ( i ) == "onOff") {
      if (server.arg ( i ) == "true") {
        onOff = true;
      }
    }
  }
  setBulbIntensity(id.toInt(), intensity.toInt(), onOff);

  server.send ( 200, "application/json", "{\"res\":\"OK\"}" );
}

void handleGetDetails() {
  server.send ( 200, "application/json", getAllDetails() );
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


