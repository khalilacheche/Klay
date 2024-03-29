
#include "HX711.h"

HX711 scale(6, 5);

float calibration_factor = 10850; // this calibration factor is adjusted according to my load cell
float units;
float ounces;

void setup() {
  DDRD |= B00011100;
  PORTD &= B00000000;                                       
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}

void loop() {

  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  Serial.print(units);
  Serial.print(" kg ");
  Serial.print("Calibration factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 50;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 50;
          if(temp == '+' || temp == '+')
      calibration_factor += 500;
    else if(temp == '-' || temp == '-')
      calibration_factor -= 500;
  }
}
