int soilMoistureValue = 0;
int soilMoisturePercent = 0;
String measuredValue = "";

//Sensor 1
const int SoilMoistureSensor1Pin = A4;
const int AirValue1 = 574;   
const int WaterValue1 = 417;  

//Sensor 2
const int SoilMoistureSensor2Pin = A3;
const int AirValue2 = 570;   
const int WaterValue2 = 411;  

//Waterpump
const int PinPump1 = 12;

void setup() {
  Serial.begin(9600); 
  pinMode(PinPump1, OUTPUT);
}
void loop() 
{
  MeasureMoisture("Sensor1", SoilMoistureSensor1Pin, AirValue1, WaterValue2);  
  delay(1000);
}

void MeasureMoisture(String name, int pin, int airValue, int waterValue)
{
  soilMoistureValue = readSoilMoisture(SoilMoistureSensor1Pin);
  soilMoisturePercent = convertToPercent(soilMoistureValue, airValue, waterValue);
  
  Serial.println("Measured on " + name + ": " + CreateLogInfo(soilMoistureValue, soilMoisturePercent));  
  
  if(soilMoistureValue < 480)
  {
    activatePump(PinPump1);
  }
  else
  {
    deactivatePump(PinPump1);
  }
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
  String measuredString = String(measuredValue);
  String percentString = String(percent);
  
  if(percent > 100) return "100% ("+ measuredString +")";
  else if(percent < 0) return "0% ("+ measuredString + ")";  
  return percentString +"% ("+ measuredString +")";
}

int readSoilMoisture(int pin)
{
  return analogRead(pin);
}
