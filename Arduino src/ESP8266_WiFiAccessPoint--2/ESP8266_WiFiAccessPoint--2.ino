#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "Default";
const char *password = "12345678";
unsigned long _time;

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
String addBulb(uint8_t id) {
  String payload = "1111,";
  payload += id;
  payload += ",";
  payload += id;
  return handleSerial(payload);
}

/**
   addBulb(uint8_t id, int intensityOn, int intensityOff)
   Code 2222
*/
String addBulb(uint8_t id, int intensityOn, int intensityOff) {
  String payload = "2222,";
  payload += id;
  payload += ",";
  payload += id;
  payload += ",";
  payload += intensityOn;
  payload += ",";
  payload += intensityOn;
  payload += ",";
  payload += intensityOff;
  payload += ",";
  payload += intensityOff;
  return handleSerial(payload);
}

/**
   removeBulb(uint8_t bulbID)
   Code 3333
*/
String removeBulb(uint8_t id) {
  String payload = "3333,";
  payload += id;
  payload += ",";
  payload += id;
  return handleSerial(payload);
}

/**
   changeBulbState(uint8_t bulbID, bool state)
   Code 4444
*/
String changeBulbState(uint8_t id, bool state) {
  String payload = "4444,";
  payload += id;
  payload += ",";
  payload += id;
  payload += ",";
  payload += state;
  payload += ",";
  payload += state;
  return handleSerial(payload);
}

/**
   getBulbState(uint8_t bulbID)
   Code 5555
*/
String getBulbState(uint8_t id) {
  String payload = "5555,";
  payload += id;
  payload += ",";
  payload += id;
  return handleSerial(payload);
}

/**
   getAllDetails()
   Code 6666
*/
String getAllDetails() {
  String payload = "6666";
  return handleSerial(payload);
}

/**
   setBulbIntensity(uint8_t bulbID, int intensity, bool onOff)
   Code 7777
*/
String setBulbIntensity(uint8_t id, int intensity, bool onOff) {
  String payload = "7777,";
  payload += id;
  payload += ",";
  payload += id;
  payload += ",";
  payload += intensity;
  payload += ",";
  payload += intensity;
  payload += ",";
  payload += onOff;
  payload += ",";
  payload += onOff;
  return handleSerial(payload);
}

/**
   Send 'a' until get responce 'b'
   After send the payload
*/
String handleSerial(String payload) {
  bool isTimeout = true;
  bool isSent = false;
  uint8_t isSuccess = 0;
  String result = "";
  _time = millis();
  while (millis() - _time < 1000) { //Set 1 second timeout
    if (!isSent) {
      Serial.print('a');
      char responceChar = Serial.read();
      if (responceChar == 'b') {
        Serial.print("ssss");
        Serial.println(payload);
        Serial.println("\n\n");
        isSent = true;
      } else {
        delay(1);
      }
    } else {
      char responceChar = Serial.read();
      if (responceChar == 'e') {
        isSuccess -= 1;
        result = "";
      } else if (responceChar == 's') {
        isSuccess += 1;
        result = "";
      } else if (responceChar == '\n') {
        Serial.print("rrrr");
        break;
      } else {
        result += responceChar;
      }
    }
  }
  if (isTimeout) {
    return "{\"res\":\"T\"}";
  } else {
    if (isSuccess > 0) {
      return "{\"res\":\"S\",\"data\":\"" + result + "\"}";
    } else {
      return "{\"res\":\"E\",\"data\":\"" + result + "\"}";
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

  String res = "";
  if (count == 1) {
    res = addBulb(id.toInt());
  } else if (count == 3) {
    res = addBulb(id.toInt(), intensityOn.toInt(), intensityOff.toInt());
  }

  server.send ( 200, "application/json", res );
}

void handleRemoveBulb() {
  String id;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    }
  }
  String res = removeBulb(id.toInt());

  server.send ( 200, "application/json", res );
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
  String res = changeBulbState(id.toInt(), state);

  server.send ( 200, "application/json", res );
}

void handleGetBulbState() {
  String id;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName ( i ) == "id") {
      id = server.arg ( i );
    }
  }
  String res = getBulbState(id.toInt());

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
  String res = setBulbIntensity(id.toInt(), intensity.toInt(), onOff);

  server.send ( 200, "application/json", res );
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


