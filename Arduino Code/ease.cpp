#include <SoftwareSerial.h>
#define RX 2
#define TX 3
#include <Wire.h> 
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//Libraries for sensors

bool BH1750Check = false;
BH1750 lightMeter;


const int voltageinputPIN2= A3;
const int voltageinputPIN = A0;
const int baudRate = 9600; //sets baud rate in bits per second for serial monitor
const int sensorreadDelay = 3050; //sensor read delay in milliseconds
const int maxanalogValue = 1010; //highest integer given at max input voltage
const int sensormaxVoltage = 25;
const int Battery = 0;
float analogVoltage = 0;
float analogVoltage2 = 0;
long time=0;
long energy;
String AP = "OPPO-31";       // AP NAME
String PASS = "12344567"; // AP PASSWORD
String API = "3I1T9M5ARLF93RK6";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
  
SoftwareSerial esp8266(RX,TX); 

#define THERMISTORPIN A2        
// resistance of termistor at 25 degrees C
#define THERMISTORNOMINAL 10000      
#define TEMPERATURENOMINAL 25   
// Accuracy - Higher number is bigger
#define NUMSAMPLES 10
// Beta coefficient from datasheet
#define BCOEFFICIENT 3950
// the value of the R1 resistor
#define SERIESRESISTOR 10000    
//prepare pole 
uint16_t samples[NUMSAMPLES];


  
void setup() {

// initialize the LCD
Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

  BH1750Check = lightMeter.begin();
  if (BH1750Check) {
    Serial.println(F("BH1750 Test begin"));
  }
  else {
    Serial.println(F("BH1750 Initialization FAILED"));
    while (true) //flow trap
    {}
  }

  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
  lcd.backlight();
}

void loop() {
//send data to server
 String getData = "GET /update?api_key="+ API +"&field4="+getBattery()+"&field3="+getTemp()+"&field1="+getLight()+"&field2="+getPower();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 delay(1500);
  int adc = analogRead(A1);
  float voltage = adc*5/1023.0;
  float c = (voltage-2.5)/0.185;
  float current = c + 1;
  
//revenue calculation
if(current > 0.15)
{
  time += 900;
}
long hr = time/3600;
getVolt2();
long Power = current * 5;
long Energy = Power *hr;
analogVoltage = analogRead(voltageinputPIN); //reads analog voltage of incoming sensor
  analogVoltage = (analogVoltage/maxanalogValue)*sensormaxVoltage;
  int Battery =(analogVoltage/4.5)*100;
  
  lcd.setCursor(0, 0);
  lcd.print("Battery :");
  lcd.print(Battery);
  lcd.setCursor(0, 1);
  lcd.print("Savings:");
  lcd.print(Energy*4.50);
  delay(5000);
}

long getPower()
{
  int adc = analogRead(A1);
  float voltage = adc*5/1023.0;
  float c = (voltage-2.5)/0.185;
  float current = c + 1;

if(current > 0.15)
{
  time += 900;
}
long hr = time/3600;
getVolt2();
long Power = 5 * current;
long Energy = Power *hr;
return long(Energy*4.50);

}

 float getBattery(){
  analogVoltage = analogRead(voltageinputPIN); //reads analog voltage of incoming sensor
  analogVoltage = (analogVoltage/maxanalogValue)*sensormaxVoltage;
  int Battery =(analogVoltage/4.5)*100 ;
  return float(Battery);
}

float getCurrent()
{
  int adc = analogRead(A1);
  float voltage = adc*5/1023.0;
  float current = (voltage-2.5)/0.185;
  return float(current);
}

float getVolt2()
{
  analogVoltage2= analogRead(voltageinputPIN2); //reads analog voltage of incoming sensor
  analogVoltage2 = (analogVoltage2/maxanalogValue)*sensormaxVoltage;
  return float(analogVoltage2);
}

float getLight()
{
  if (BH1750Check) {
    int lux = lightMeter.readLightLevel();
    return int(lux); 
  }
}

float getTemp()
{
  uint8_t i;
  float average;
 
 // saving values from input to pole
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
   // average value of input
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  //resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

 //resistence to temperature
  float temperature;
  temperature = average / THERMISTORNOMINAL; 
  temperature = log(temperature);   
  temperature /= BCOEFFICIENT;                  
  temperature += 1.0 / (TEMPERATURENOMINAL + 273.15); 
  temperature = 1.0 / temperature;                 
  temperature -= 273.15; 
  return float(temperature);  
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found=false;
}