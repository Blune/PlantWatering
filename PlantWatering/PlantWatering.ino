int soilMoistureValue = 0;
int soilMoisturePercent = 0;

//Sensor 1
const int SoilMoistureSensor1Pin = A4;
const int AirValueSensor1 = 574;   
const int WaterValueSensor1 = 417;  

//Sensor 2
const int SoilMoistureSensor2Pin = A3;
const int AirValueSensor2 = 570;   
const int WaterValueSensor2 = 411;  

//Sensor 3
const int SoilMoistureSensor3Pin = A3;
const int AirValueSensor3 = 570;   
const int WaterValueSensor3 = 411;  

//Sensor 4
const int SoilMoistureSensor4Pin = A3;
const int AirValueSensor4 = 570;   
const int WaterValueSensor4 = 411;  

//Waterpump
const int PinPump1 = 12;
const int PinPump2 = 10;
const int PinPump3 = 11;
const int PinPump4 = 13;

void setup() {
  Serial.begin(9600); 
  pinMode(PinPump1, OUTPUT);
  deactivatePump(PinPump1);
  pinMode(SoilMoistureSensor1Pin,INPUT);
  //TODO: Deactivate other pins when electronic done
}
void loop() 
{
  checkPlant("Sensor1", SoilMoistureSensor1Pin, AirValueSensor1, WaterValueSensor1, 30);  
  checkPlant("Sensor2", SoilMoistureSensor2Pin, AirValueSensor2, WaterValueSensor2, 30);
  checkPlant("Sensor3", SoilMoistureSensor3Pin, AirValueSensor3, WaterValueSensor3, 30);
  checkPlant("Sensor4", SoilMoistureSensor4Pin, AirValueSensor4, WaterValueSensor4, 30);
  delay(3000);
}

void checkPlant(String name, int pin, int airValue, int waterValue, int moisturePercent)
{
  soilMoistureValue = readSoilMoisture(SoilMoistureSensor1Pin);
  soilMoisturePercent = convertToPercent(soilMoistureValue, airValue, waterValue);
  
  Serial.println("Measured on " + name + ": " + CreateLogInfo(soilMoistureValue, soilMoisturePercent));  
  
  if(soilMoisturePercent < moisturePercent)
  {
    Serial.println("Moisture too low. Started water pump for 2 seconds.");
    activatePump(PinPump1);    
    delay(2000);
    deactivatePump(PinPump1);    
    Serial.println("Stopped pump");
  }  
}

int readSoilMoisture(int pin)
{
  return analogRead(pin);
}

void activatePump(int pin)
{
  digitalWrite(12, LOW);
}

void deactivatePump(int pin)
{
  digitalWrite(pin, HIGH);
}

int convertToPercent(int value, int airValue, int waterValue)
{
  return map(soilMoistureValue, airValue, waterValue, 0, 100);
}

String CreateLogInfo(int measuredValue, int percent)
{   
  if(percent >= 100) return "100% ("+ String(measuredValue) +")";
  else if(percent <= 0) return "0% ("+ String(measuredValue) + ")";  
  return String(percent) +"% ("+ String(measuredValue) +")";
}
