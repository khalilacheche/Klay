
#include "HX711.h"

HX711 scale(6, 5);
bool hasStarted =false;
float peakValue=0;
float currentMeasurment;
int previousMeasurment;
float calibration_factor = 10850;
unsigned long punchStartTime;
float units;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  scale.set_scale();//Setting up the load sensor
  scale.tare();
  scale.set_scale(calibration_factor);
}

void loop() {
  units = scale.get_units(), 10; //Read measurment from sensor
  currentMeasurment =units ;
  if(currentMeasurment>4){
    if(!hasStarted){
        peakValue=currentMeasurment;
        hasStarted=true;
        punchStartTime=millis();
    }else{
        if(previousMeasurment<currentMeasurment && currentMeasurment>peakValue){
            peakValue=currentMeasurment;
          }
   }
  }else{
    if(hasStarted){
        Serial.print("Peak Value: ");
        Serial.print(peakValue);
        unsigned long now =millis();
        Serial.print(" Punch Duration: ");
        Serial.print(now-punchStartTime);
        hasStarted=false;
        peakValue=0;  
    }
  }
  previousMeasurment = currentMeasurment;
}
