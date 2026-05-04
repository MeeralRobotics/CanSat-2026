#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

#define LORA_SS   10
#define LORA_RST  9
#define LORA_DIO0 2

#define GNSS Serial1

TinyGPSPlus gps;
int counter = 0;
Adafruit_BMP280 bmp;

void setup(){
  Serial.begin(9600);
  GNSS.begin(115200);


  if (!bmp.begin(0x76)) {
    Serial.println("no bmp");
  }

  Serial.println("bmp init");

  
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(869.4E6)) {
    Serial.println("no lora");
  }

  // lora settings
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12); // set this to a lower level
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();
  LoRa.setSyncWord(0xF3);

  Serial.println("lora init");
  
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A17, INPUT);
}

double ConvertTemp(double x){
  return (((((-1.4078e-12 * x + 3.8668e-09) * x - 4.2095e-06) * x + 2.2864e-03) * x - 7.2374e-01) * x + 1.5920e+02);
}

void loop(){
  while(GNSS.available())
  {
    gps.encode(GNSS.read());
  }
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float longitude = gps.location.lng();
  float latitude = gps.location.lat();
  double readNtc1 = analogRead(A8);
  double readNtc2 = analogRead(A9);
  double readNtc3 = analogRead(A17);

  double ntc1 = ConvertTemp(readNtc1);
  double ntc2 = ConvertTemp(readNtc2);
  double ntc3 = ConvertTemp(readNtc3);

  Serial.print(ntc1);
  Serial.print(" ");
  Serial.print(ntc2);
  Serial.print(" ");
  Serial.println(ntc3);

  // change this to lora.write for bytes not string
  LoRa.beginPacket();

  

  LoRa.endPacket();

  // Debug
  Serial.print(counter);
  Serial.print(' ');
  Serial.print(temp);
  Serial.print(' ');
  Serial.print(pressure);
  Serial.println(' ');
 
  delay(500);
  counter++;
}