#include <eRCaGuy_analogReadXXbit.h>
#include <SoftwareSerial.h>
//SoftwareSerial SoftSerial(10, 11); // Rx,Tx
#define SoftSerial Serial

eRCaGuy_analogReadXXbit adc;
const uint8_t pin = A0;
const float MAX_READING_14_bit = 16368.0;
uint8_t bits_of_precision;
float analog_reading;
const unsigned long num_samples = 15;
const float drop_presicision = 200;
int ar = 0;
long prev_read_time= 0;
int prev_read_val= 0;
String state = "11";
void setup() {
  
  delay(2000);
  Serial.begin(115200);
  SoftSerial.begin(115200);
}

void loop() {
  
  bits_of_precision = 14;
  analog_reading = adc.analogReadXXbit(pin, bits_of_precision, num_samples);
  String str = "";
  if(ar==0){
     ar = (int) analog_reading;
     prev_read_val = ar;
     prev_read_time = millis();
  }
  else if(abs((int) analog_reading - ar)< drop_presicision){
    ar = (int) analog_reading;
  }
   
  str += (String) ar;
  if(str.length()==4){
    str = '0' + str;
  }
  else if(str.length()==3){
    str = "00" + str;
  }
  else if(str.length()==2){
    str = "000" + str;
  }
  else if(str.length()==1){
    str = "0000" + str;
  }
  else if(str.length()==0){
    str = "00000" + str;
  }
  //Serial.println(str);
  if(abs(millis()-prev_read_time) > 300000){
    if(ar>prev_read_val){
      state="11";
    }
    if(ar<prev_read_val){
      state="00";
    }

    prev_read_time = millis();
    prev_read_val = ar;
  }
  if (ar != 0) {
    handleSerial(str+state);
  }

  
}

void handleSerial(String analog_read) {
  SoftSerial.print("a");
  SoftSerial.print(analog_read);
  //Serial.println(analog_read);
  
 //SoftSerial.print("bbbbb");
  //SoftSerial.print(analog_read);
//  SoftSerial.println("b);
//  SoftSerial.println("b");
//  SoftSerial.println("b");

}
