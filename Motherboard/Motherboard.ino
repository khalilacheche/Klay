#include <Wire.h>;
#include <SoftwareSerial.h>;

int NB_SENSORS = 5;
SoftwareSerial bluetoothPort(2, 3);

int scenario;
int scenarios [][10]={ //the Hard-coded sequences
                      {0,1,2,3,4,0,1,2,3,4},
                      {1,2,4,2,3,3,1,0,1,0},
                      {1,0,3,0,1,2,0,4,1,4}
                    };
void setup() {
  //Beginning the I2C, USB Serial and Bluetooth Serial communication
  Wire.begin();
  bluetoothPort.begin(9600);
  Serial.begin(9600);
  
  delay(1000);
}

void loop() {
  scenario = selectScenario(); //Select the scenario
  startCountDown(3); //Start 3 seconds countdown
  String results = executeScenario(scenarios[scenario]); //Execute selected Scenario
  sendResults(results); // Send the results
  flushExcess(); // Flush down the excess data that might have come during the execution
}


String executeScenario(int scenario[]){
  String results="{\"data\":[";
  //int steps = sizeof(scenario) / sizeof(scenario[0]); // Array length
  for(int i = 0; i<8;i++){
      Serial.print("Getting Results from SensorBoard number ");
      Serial.print(scenario[i]);
      Serial.println();
      //results +=getResultfromSensor(scenario[i])+"\n";
      String resp=getResultfromSensor(scenario[i]);
      Serial.println(resp);
      results +=transformToJson(resp);
      if(i!=7){
        results+=",";
      }
      Serial.println("Done");
    }
  results+="]}"; 
  Serial.println(results);
  return results;
}
String transformToJson(String in){
  String out="{\"hasPunched\":";
  if(in.charAt(0)=='0'){
    out+="false" ;
   }else{
     out+="true";
   }
  out+=",\"peakValue\":";
  out+=in.substring(2,6);
  out+=",\"reactionTime\":";
  out+=in.substring(7,11);
  out+=",\"id\":";
  out+=in.substring(11);
  out+="}";
  return out;
}
String getResultfromSensor(int device){
    Wire.beginTransmission((device+8)); // Requesting the SensorBoard to start monitoring
    Wire.write("a"); 
    Wire.endTransmission();
    Serial.println("Begin monitoring");
    delay(2000);
    Wire.requestFrom((device+8), 13); //Telling the Sensorboard to send the result
    Serial.println("End monitoring");
    String result;   
    while (Wire.available()) {
      char c = Wire.read(); // receive a byte as character
      Serial.print(c);
      result= result+c; // Appending the char to the rest of the response
    }
    Serial.println();
    return result;
}

void sendResults(String results){
  Serial.println("Sending results...");
  bluetoothPort.println(results);
  bluetoothPort.println("End"); //End signal
  Serial.print("End of Sequence");
}
int selectScenario() {
  int resultat=-1;
  boolean hasSelected = false;
  while(!hasSelected){
    int inByte=-1;
    if (bluetoothPort.available()>0) {
      inByte = bluetoothPort.read()-49;//Receiving ASCII code, substracting 49 to find numerical value
      Serial.println(inByte);
    }
    if (inByte<3 && inByte>-1){
      resultat = inByte;
      hasSelected=true;
     }
  }
  
  //bluetoothPort.print("Selected Scenario: ");
  //bluetoothPort.println(resultat);
  Serial.print("Selected Scenario: ");
  Serial.println(resultat);
  return resultat;
}
void flushExcess(){
    while(bluetoothPort.available()>0) {
      Serial.print("Flushed down 1 byte");
      String inByte;
      inByte = bluetoothPort.read();
    }
  }
void startCountDown(int start){
  for(int i =0 ; i<NB_SENSORS;i++){//Telling all sensorboards to start blinking
    Wire.beginTransmission((i+8));
    Wire.write("b");
    Wire.endTransmission();
  }
  for(int i=start; i>0;i--){
    //bluetoothPort.println(i);
    Serial.println(i);
    delay(1000);
  }
  for(int i =0 ; i<NB_SENSORS;i++){//Telling all sensoboards to stop blinking
    Wire.beginTransmission((i+8));
    Wire.write("c");
    Wire.endTransmission();
  }
  Serial.println("Go!");
  bluetoothPort.println("!!!,");//Telling the smartphone the sequence started
  
}
