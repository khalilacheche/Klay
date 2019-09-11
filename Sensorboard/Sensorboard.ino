#include <Wire.h>
#include "HX711.h"

HX711 scale(6, 5);


int ADDRESS=8;

int RED = 2;
int GREEN = 4;
int BLUE = 3;
float calibration_factor = 10850; // this calibration factor is adjusted according to my load cell
float units;
int reactionTime=0;

int peakValue=0;
boolean hasPunched=false;
boolean hasStarted=false;
int currentMeasurment=0;
int previousMeasurment = 0;

boolean startedMonitoring=false;
unsigned long startTime=0;

boolean isBlinking;
unsigned long previousMillis;
boolean ledState;

void setup() {
  Serial.begin(9600);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  setLedBlue();
  Wire.begin(ADDRESS);                // join i2c bus with address #8
  Wire.onRequest(sendData); // register event
  Wire.onReceive(receiveData); // register event
  scale.set_scale();//Setting up the load sensor
  scale.tare();
}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  units = scale.get_units(), 10; //Read measurment from sensor
  currentMeasurment = (int)(units * 100);

  if(isBlinking){
    blinkNoDelay(500);
    return;
  }
  if(!startedMonitoring){
      setLedOff();
      return;
    }
  checkForPunch();
    
}

void receiveData(int len){
  char c;
  while(Wire.available()>0) 
    {
      // read byte as a character
      c = Wire.read();
    }
  switch(c){
      case 'a':
        startedMonitoring=true;
        startTime=millis();
        break;
      case 'b':
        isBlinking=true;
        ledState=false;
        previousMillis=millis();
        setLedOff();
        break;
      case 'c':
        isBlinking = false;
        ledState=false;
        break;
    
    }  

}

void sendData() {
  String result;
  if(hasPunched){
    result+="1";
  }else{//If there's no punch, we zero out everything and set the led red for 1 sec
    peakValue=0;
    reactionTime=0;
    for(int i=0;i<500;i++){
        setLedRed();
        delayMicroseconds(250);delayMicroseconds(250);delayMicroseconds(250);delayMicroseconds(250);
    }
    result+="0";
  }
  String a = String(peakValue);
  for(int i=0 ;i<a.length()-4;i++){//Making sure the string is always the same length no matter how long the number is
    a=" "+a;
  }
  result += a;
  //result+=",\"peakValue\":"+a;
  String b = String(reactionTime);
  for(int i=0 ;i<b.length()-4;i++){//Making sure the string is always the same length no matter how long the number is
    b=" "+b;
  }
  result+=" ";
  result +=b;
  String id=String((ADDRESS-8));
  result+=" ";
  result+=id;
  Wire.write(result.c_str());//Sending back the result
  peakValue=0;//Resetting all values
  hasPunched=false;
  startedMonitoring=false;
  reactionTime=0;
}
void setLedGreen(){
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,HIGH);
}
void setLedRed(){
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
}
void setLedBlue(){
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,LOW);
}
void setLedWhite(){
  digitalWrite(RED,LOW);
  digitalWrite(GREEN,LOW);
  digitalWrite(BLUE,LOW);
}
void setLedOff(){
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
}
void checkForPunch(){
    if(hasPunched){
      setLedGreen();
    }else{
      setLedBlue(); 
    }
  
  if(currentMeasurment>150){//if we are above the thresold
    if(!hasStarted){ // and the punch hasn't started yet, that means that the punch started now
        peakValue=currentMeasurment;
        hasStarted=true;
        reactionTime = millis()-startTime;
    }else{//and the punch has already started, we just update the peakvalue if necessary
        if(previousMeasurment<currentMeasurment && currentMeasurment>peakValue){
            peakValue=currentMeasurment;
          }
   }
  }else{
    if(hasStarted){//if we are below the threshold and the punch has started, that means the punch is over
        setLedGreen();
        hasStarted=false;
        hasPunched=true;
    }
  }
  previousMeasurment = currentMeasurment;
  
}
void blinkNoDelay(int interval){
  unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    ledState=!ledState;
    if (ledState == HIGH) {
      setLedWhite();
    } else {
      setLedOff();
    }
  }
}
