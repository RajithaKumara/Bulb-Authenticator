#include <SoftwareSerial.h>

SoftwareSerial SoftSerial(10, 11); // Rx,Tx

struct Bulb {
  uint8_t id;
  int intensityOn;
  int intensityOff;
  bool state;
};
uint8_t bulbCount = 0;
Bulb bulbs[] = {};
int intensity = 0;
unsigned long time;

void setup() {
  delay(1000);
  Serial.begin(115200);
  SoftSerial.begin(115200);
}

void loop() {
  if (SoftSerial.available()) {
    Serial.println("handle s serial");
    handleSerial();
  }

  Serial.println("reading intensity");
  Serial.println(intensity);
  delay(50);
  intensity += 1; //assign intensity value
  Serial.println("finished reading intensity");
}


/**
   Functions
*/

/**
   addBulb(uint8_t id)
   Code 1111
*/
void addBulb(uint8_t id) {
  Bulb bulb = {id, 0, 0, false};
  bulbs[bulbCount] = bulb;
  bulbCount += 1;
}

/**
   addBulb(uint8_t id, int intensityOn, int intensityOff)
   Code 2222
*/
void addBulb(uint8_t id, int intensityOn, int intensityOff) {
  Bulb bulb = {id, intensityOn, intensityOn, false};
  bulbs[bulbCount] = bulb;
  bulbCount += 1;
}

/**
   removeBulb(uint8_t bulbID)
   Code 3333
*/
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

/**
   changeBulbState(uint8_t bulbID, bool state)
   Code 4444
*/
void changeBulbState(uint8_t bulbID, bool state) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      bulb.state = state;
    }
  }
}

/**
   getBulbState(uint8_t bulbID)
   Code 5555
*/
bool getBulbState(uint8_t bulbID) {
  Bulb bulb = findById(bulbID);
  return bulb.state;
}

/**
   getAllDetails()
   Code 6666
*/
String getAllDetails() {
  String json = "[";
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    json += "{\"id\":\"";
    json += bulb.id;
    json += "\",\"intensityOn\":\"";
    json += bulb.intensityOn;
    json += "\",\"intensityOff\":\"";
    json += bulb.intensityOff;
    json += "\",\"state\":\"";
    json += bulb.state;
    json += "\"}";
  }
  json += "]";
  return json;
}

/**
   setBulbIntensity(uint8_t bulbID, int intensity, bool onOff)
   Code 7777
*/
void setBulbIntensity(uint8_t bulbID, int intensity, bool onOff) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      if (onOff) {
        bulb.intensityOn = intensity;
      } else {
        bulb.intensityOff = intensity;
      }
    }
  }
}

Bulb findById(uint8_t bulbID) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      return bulb;
    }
  }
}

void handleSerial() {
  char requestChar = SoftSerial.read();

  if (requestChar == 'a') { //Detect first request with 'a'
    String payload = "";

    time = millis();
    while (millis() - time < 1000) { //Collect char values
      requestChar = SoftSerial.read();

      if (requestChar == 'a') {
        SoftSerial.print('b'); //Responce 'b' for while 'a' request
        payload = "";
      } else if (requestChar == 's') {
        payload = "";
      } else if (requestChar == '\n') {
        break;
      }
      payload += requestChar ;
    }

    functionHandler(payload);
  }
}

void functionHandler(String payload) {
  String function = payload.substring(1, 5);

  Serial.println("##########################################");
  Serial.println(payload);
  Serial.println(function);
  Serial.println("##########################################");

  switch (function.toInt()) {
    case 1111:
      //addBulb
      Serial.println("Add bulb");
      break;
    case 2222:
      //addBulb
      break;
    case 3333:
      //removeBulb
      break;
    case 4444:
      //changeBulbState
      break;
    case 5555:
      //getBulbState
      break;
    case 6666:
      //getAllDetails
      break;
    case 7777:
      //setBulbIntensity
      break;
    default:
      serialErrorHandler();
  }
}

void serialErrorHandler() {
  Serial.println("Error...");
}


