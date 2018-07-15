struct Bulb {
  uint8_t id;
  int intensity;
  bool state;
};
uint8_t bulbCount = 0;
Bulb bulbs[] = {};
int intensity = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);

}

void loop() {
//  if (Serial.available()) {
//    Serial.write(Serial.read());
//    Serial.println(Serial.read());
//  }
  while(Serial.available()){
    Serial.println(Serial.read());
  }

  Serial.println("reading intensity");
  delay(500);
  intensity = 100; //assign intensity value
  Serial.println("finished reading intensity");
  
}

/**
   Functions
*/
void addBulb(uint8_t id) {
  Bulb bulb = {id, 0, false};
  bulbs[bulbCount] = bulb;
  bulbCount += 1;
}

void addBulb(uint8_t id, int intensity) {
  Bulb bulb = {id, intensity, false};
  bulbs[bulbCount] = bulb;
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
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      bulb.state = state;
    }
  }
}

void setBulbIntensity(uint8_t bulbID, int intensity) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      bulb.intensity = intensity;
    }
  }
}

bool getBulbState(uint8_t bulbID) {
  Bulb bulb = findById(bulbID);
  return bulb.state;
}

Bulb findById(uint8_t bulbID) {
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    if (bulb.id == bulbID) {
      return bulb;
    }
  }
}

String getAllDetails() {
  String json = "[";
  for (int i = 0; i < bulbCount; i++) {
    Bulb bulb = bulbs[i];
    json += "{\"id\":\"";
    json += bulb.id;
    json += "\",\"intensity\":\"";
    json += bulb.intensity;
    json += "\",\"state\":\"";
    json += bulb.state;
    json += "\"}";
  }
  json += "]";
  return json;
}
