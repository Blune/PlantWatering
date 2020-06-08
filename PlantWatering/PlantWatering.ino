// LiquidCrystal_I2C.h: https://github.com/johnrickman/LiquidCrystal_I2C
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

int soilMoistureValue = 0;
int soilMoisturePercent = 0;
int waterLevel = 0;

//Sleep
int sleep = 30000;

//Sensor 1
const int SoilMoistureSensor1Pin = A0;
const int AirValueSensor1 = 880;   
const int WaterValueSensor1 = 565;  
const int SoilMoistureForWatering1 = 30; //basil

//Sensor 2
const int SoilMoistureSensor2Pin = A3;
const int AirValueSensor2 = 570;   
const int WaterValueSensor2 = 411;  
const int SoilMoistureForWatering2 = 30; //basil

//Sensor 3
const int SoilMoistureSensor3Pin = A3;
const int AirValueSensor3 = 570;   
const int WaterValueSensor3 = 411;  
const int SoilMoistureForWatering3 = 30; //basil

//Sensor 4
const int SoilMoistureSensor4Pin = A3;
const int AirValueSensor4 = 570;   
const int WaterValueSensor4 = 411; 
const int SoilMoistureForWatering4 = 30; //basil 

//Sensor 5 Water level check
const int WaterLevelSensorPin = A6;
const int AirValueWaterLevel = 883;
const int WaterValueWaterLevel = 470;
const int MinWaterValueWaterLevelSensor = 25; //25% of water tank

//Waterpump
const int PinPump1 = 9;
const int PinPump2 = 7;
const int PinPump3 = 8;
const int PinPump4 = 5;

//display
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 4);

void setup() {
  Serial.begin(9600); 
  pinMode(PinPump1, OUTPUT);
  pinMode(SoilMoistureSensor1Pin,INPUT);
  deactivatePump(PinPump1);  
  //TODO: Deactivate other pins when electronic done

  lcd.init();
  lcd.backlight();
}
void loop() 
{
  writeToDisplay(0,0, "Basil: "+ checkPlant("Sensor1", SoilMoistureSensor1Pin, AirValueSensor1, WaterValueSensor1, PinPump1, SoilMoistureForWatering1)); 
  writeToDisplay(0,1, "Minze: "+ checkPlant("Sensor2", SoilMoistureSensor2Pin, AirValueSensor2, WaterValueSensor2, PinPump2, SoilMoistureForWatering2)); 
  //TODO: Find bug why text in third row is shifted to the right
  writeToDisplay(-4,2, "Chilli: "+ checkPlant("Sensor3", SoilMoistureSensor3Pin, AirValueSensor3, WaterValueSensor3, PinPump3, SoilMoistureForWatering3)); 
  writeToDisplay(-4,3, "Wurst: "+ checkPlant("Sensor4", SoilMoistureSensor4Pin, AirValueSensor4, WaterValueSensor4, PinPump4, SoilMoistureForWatering4));  
  writeWaterLevelToDisplay();
  delay(sleep);
}

void writeToDisplay(int col, int row, String text)
{
  lcd.setCursor(col, row);
  lcd.print(text);
}

void writeWaterLevelToDisplay()
{
  waterLevel = readSensor(WaterLevelSensorPin);
  int waterLevelPercent = convertToPercent(waterLevel, AirValueWaterLevel, WaterValueWaterLevel);
  Serial.println("measured water level: "+ String(waterLevel)+"("+ String(waterLevelPercent)+ "%)");
  //writeToDisplay(15,0,"▮");
  lcd.setCursor(15, 0);
  lcd.write(255);
  lcd.setCursor(15, 1);
  lcd.write(255);
  lcd.setCursor(11, 2);
  lcd.write(255);
  lcd.setCursor(11, 3);
  lcd.write(255);
  //TODO: Find bug why text in third row is shifted to the right
}

String checkPlant(String name, int pin, int airValue, int waterValue, int pinPump, int moisturePercentForWatering)
{
  soilMoistureValue = readSensor(SoilMoistureSensor1Pin);
  soilMoisturePercent = convertToPercent(soilMoistureValue, airValue, waterValue);

  Serial.println("Measured on " + name + ": " + CreateLogInfo(soilMoistureValue, soilMoisturePercent));  

  //Moved for testing purposes before if statement
  bool isEnoughWater = isEnoughWaterInTank();
  if(soilMoisturePercent < moisturePercentForWatering && isEnoughWater)
  {    
    Serial.println("Moisture too low. Started water pump for 2 seconds on pin " + String(pinPump));
    activatePump(PinPump1);    
    delay(500);
    deactivatePump(PinPump1);    
    Serial.println("Stopped pump");
  }

    return CreateLogInfo(soilMoistureValue, soilMoisturePercent);
}

bool isEnoughWaterInTank()
{
  waterLevel = readSensor(WaterLevelSensorPin);
  int waterLevelPercent = convertToPercent(waterLevel, AirValueWaterLevel, WaterValueWaterLevel);
  Serial.println("measured water level: "+ String(waterLevel)+"("+ String(waterLevelPercent)+ "%)");
  bool isEnoughWater = waterLevelPercent >= MinWaterValueWaterLevelSensor;
  if(isEnoughWater && digitalRead(LED_BUILTIN) == HIGH)
  {
     Serial.println("Enough water at level: "+ String(waterLevelPercent)+ "%"); 
    digitalWrite(LED_BUILTIN, LOW); 
  }
  else if(!isEnoughWater && digitalRead(LED_BUILTIN) == LOW)
  {
    Serial.println("Water level is below minimum of "+ String(waterLevelPercent)+ "%"); 
    digitalWrite(LED_BUILTIN, HIGH);     
  }
  return isEnoughWater;
}

int readSensor(int pin)
{
  return analogRead(pin);
}

void activatePump(int pin)
{
  digitalWrite(pin, HIGH);
}

void deactivatePump(int pin)
{
  digitalWrite(pin, LOW);
}

int convertToPercent(int value, int airValue, int waterValue)
{
  return map(soilMoistureValue, airValue, waterValue, 0, 100);
}

String CreateLogInfo(int measuredValue, int percent)
{     
  String measuredText = String(measuredValue);
  Serial.println("Water level is below minimum of "+ String(percent) +"% ("+ measuredText); 
  
  if(percent >= 100) return "100%";
  else if(percent <= 0) return "0%";  
  return String(percent) +"%";
}
