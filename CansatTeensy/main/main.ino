#include "ntc.h"
#include "bmp.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <LoRa.h>


#define LORA_SS   10
#define LORA_RST  9
#define LORA_DIO0 2

int counter = 0;
Adafruit_BMP280 bmp;

void setup(){
  Serial.begin(9600);

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
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.enableCrc();
  LoRa.setSyncWord(0xF3);

  Serial.println("lora init");


  
}

void loop(){
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure();
  int ntc = analogRead(A9);

  // change this to lora.write for bytes not string
  LoRa.beginPacket();

  LoRa.print(counter);
  LoRa.print(' ');
  LoRa.print(temp);
  LoRa.print(' ');
  LoRa.print(pressure);
  LoRa.print(' ');
  LoRa.print(ntc);

  LoRa.endPacket();

  // Debug
  Serial.print(counter);
  Serial.print(' ');
  Serial.print(temp);
  Serial.print(' ');
  Serial.print(pressure);
  Serial.print(' ');
  Serial.println(ntc);

  counter++;

  delay(1000);
}