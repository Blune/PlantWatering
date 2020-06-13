// LiquidCrystal_I2C.h: https://github.com/johnrickman/LiquidCrystal_I2C
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

//Init variables
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
const int MinWaterPercentage = 25; //25% of water tank

//Waterpump
const int PinPump1 = 9;
const int PinPump2 = 7;
const int PinPump3 = 8;
const int PinPump4 = 5;

//display
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 4);

void setup() 
{
  Serial.begin(9600); 
  pinMode(PinPump1, OUTPUT);
  pinMode(PinPump2, OUTPUT);
  pinMode(PinPump3, OUTPUT);
  pinMode(PinPump4, OUTPUT);
  
  pinMode(SoilMoistureSensor1Pin,INPUT);
  pinMode(SoilMoistureSensor2Pin,INPUT);
  pinMode(SoilMoistureSensor3Pin,INPUT);
  pinMode(SoilMoistureSensor4Pin,INPUT);
  pinMode(WaterLevelSensorPin,INPUT);
  
  deactivatePump(PinPump1);  
  deactivatePump(PinPump2);  
  deactivatePump(PinPump3);  
  deactivatePump(PinPump4);  
  
  lcd.init();
  lcd.backlight();
}

void loop() 
{  
  writeToDisplay(0,0, checkPlant("Basil", SoilMoistureSensor1Pin, AirValueSensor1, WaterValueSensor1, PinPump1, SoilMoistureForWatering1)); 
  writeToDisplay(0,1, checkPlant("Minze", SoilMoistureSensor2Pin, AirValueSensor2, WaterValueSensor2, PinPump2, SoilMoistureForWatering2)); 
    //TODO: Find bug why text in third row is shifted to the right
  writeToDisplay(-4,2, checkPlant("Chilli", SoilMoistureSensor3Pin, AirValueSensor3, WaterValueSensor3, PinPump3, SoilMoistureForWatering3)); 
  writeToDisplay(-4,3, checkPlant("Wurst", SoilMoistureSensor4Pin, AirValueSensor4, WaterValueSensor4, PinPump4, SoilMoistureForWatering4));  
  
  writeWaterLevelToDisplay();
  delay(sleep);
}

String checkPlant(String name, int pin, int airValue, int waterValue, int pinPump, int moisturePercentForWatering)
{
  soilMoistureValue = readSensor(SoilMoistureSensor1Pin);
  soilMoisturePercent = convertToPercent(soilMoistureValue, airValue, waterValue);
  Serial.println("Measured on " + name + ": " + createPercentageText(soilMoisturePercent));  

  if(soilMoisturePercent < moisturePercentForWatering && isEnoughWaterInTank()) activatePump(pinPump);

  return adjustNameTo8Chars(name) + createPercentageText(soilMoisturePercent);
}

void activatePumpFor500ms(int pinPump)
{
    Serial.println("Moisture too low. Started water pump for 2 seconds on pin " + String(pinPump));
    activatePump(pinPump);    
    delay(500);
    deactivatePump(pinPump);    
    Serial.println("Stopped pump");  
}

bool isEnoughWaterInTank()
{
  int waterLevelPercent = measureWaterTankPercentage();
  Serial.println("measured water level: "+ String(waterLevelPercent)+ "% and min set to "+ String(MinWaterPercentage) + "%" );
  return waterLevelPercent >= MinWaterPercentage;
}

void writeToDisplay(int col, int row, String text)
{
  lcd.setCursor(col, row);
  lcd.print(text);
}

void writeWaterLevelToDisplay()
{
  int waterLevelPercent = measureWaterTankPercentage();
  waterLevelPercent = waterLevelPercent; 
  Serial.println("measured water level: " + String(waterLevelPercent)+ "%)");

  lcd.setCursor(14, 0);
  lcd.print(' ');
  if(waterLevelPercent > 80)lcd.write(255);  

  lcd.setCursor(14, 1);
  lcd.print(' ');
  if(waterLevelPercent > 60)lcd.write(255);  

  //TODO: Find bug why text in third row is shifted to the right
  lcd.setCursor(10, 2);
  lcd.print(' ');
  if(waterLevelPercent > 40)lcd.write(255);
  
  //TODO: Find bug why text in third row is shifted to the right  }
  lcd.setCursor(10, 3);
  lcd.print(' ');
  if(waterLevelPercent > 20)lcd.write(255);  
}

int measureWaterTankPercentage()
{
  waterLevel = readSensor(WaterLevelSensorPin);
  return convertToPercent(waterLevel, AirValueWaterLevel, WaterValueWaterLevel);  
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

String createPercentageText(int percent)
{    
  //Always return 000% for same length to override chars on display correct 
  if(percent >= 100) return "100%";
  else if(percent <= 0) return "  0%";  
  else if(percent < 10) return "  " + String(percent) +"%";
  return " " + String(percent) +"%";
}

String adjustNameTo8Chars(String s)
{
  if(s.length() > 8) return s.substring(0,8);
  else
  {
    for(int i = s.length(); i <= 8; i++)s += ' ';        
    return s;
  }
}
  
