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
  serialSuccessHandler(json);
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

/**
 * Handle serial comm with wifi module
 */
void handleSerial() {
  char requestChar = SoftSerial.read();

  if (requestChar == 'a') { //Detect first request with 'a'
    String payload = "";
    bool isTimeout = true;

    time = millis();
    while (millis() - time < 50) { //Collect char values
      requestChar = SoftSerial.read();

      if (requestChar == 'a') {
        SoftSerial.print('b'); //Responce 'b' for while 'a' request
        payload = "";
      } else if (requestChar == 's') {
        payload = "";
      } else if (requestChar == '\n') {
        isTimeout = false;
        break;
      }
      payload += requestChar ;
    }
    if (isTimeout) {
      serialErrorHandler(1);//Error code 1 for timeout
    } else {
      functionHandler(payload);
    }
  }
}

/**
 * Navigate to functions
 */
void functionHandler(String payload) {
  String function = payload.substring(1, 5);
  String variableStr = payload.substring(6, payload.length());

  Serial.println("##########################################");
  Serial.println(payload);
  Serial.println(function);
  Serial.println(variableStr);
  Serial.println("##########################################");

  uint8_t count = 1;
  int variables[] = {};
  String tempVal = "";
  uint8_t index = variableStr.indexOf(',');

  time = millis();
  while (!(index == 255) && millis() - time < 10) {
    if ((count % 2) == 1) {
      tempVal = variableStr.substring(0, index);
    } else {
      if (tempVal == variableStr.substring(0, index)) {
        variables[count / 2] = tempVal.toInt();
      } else {
        serialErrorHandler(2);//error code 2 for error in variables
        break;
      }
    }
    variableStr = variableStr.substring(index + 1, variableStr.length());
    count += 1;
    index = variableStr.indexOf(',');
  }
  variables[0] = count / 2;

  switch (function.toInt()) {
    case 1111:
      //addBulb
      if (variables[0] == 1) {
        addBulb(variables[1]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 2222:
      //addBulb
      if (variables[0] == 3) {
        addBulb(variables[1]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 3333:
      //removeBulb
      if (variables[0] == 1) {
        removeBulb(variables[1]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 4444:
      //changeBulbState
      if (variables[0] == 2) {
        changeBulbState(variables[1], variables[2]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 5555:
      //getBulbState
      if (variables[0] == 1) {
        getBulbState(variables[1]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 6666:
      //getAllDetails
      if (variables[0] == 0) {
        getAllDetails();
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    case 7777:
      //setBulbIntensity
      if (variables[0] == 2) {
        setBulbIntensity(variables[1],intensity,variables[2]);
      } else {
        serialErrorHandler(4);//error code 4 for invalid variable count
      }
      break;
    default:
      serialErrorHandler(3);//error code 3 for invalid payload
  }
}

/**
 * Serial comm error handling
 */
void serialErrorHandler(uint8_t code) {
  switch (code) {
    case 1:
      //Error code 1 for timeout
      Serial.println("Time Out...");
      break;
    case 2:
      //Error code 2 for error in variables
      Serial.println("Error in variables");
      break;
    case 3:
      //Error code 3 for invalid payload
      Serial.println("Invalid_payload...");
      break;
    case 4:
      //Error code 4 for invalid variable count
      Serial.println("Invalid variable count");
      break;
    default:
      Serial.println("Default Error...");
  }
}

/**
 * Serial comm success handling
 */
void serialSuccessHandler(String payload) {
  Serial.println("Success...  :" + payload);
}


