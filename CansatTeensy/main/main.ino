#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <SD.h>
#include "MPU6050_6Axis_MotionApps20.h"

#define LORA_SS   10
#define LORA_RST  9
#define LORA_DIO0 2
#define buzzer 37
#define GNSS Serial1
#define OUTPUT_READABLE_QUATERNION

TinyGPSPlus gps;
int counter = 0;
Adafruit_BMP280 bmp;
MPU6050 mpu;
float seaLevelPressure = 0.0;
Quaternion q;

uint8_t fifoBuffer[64];

File f;
char filename[15];

double lat = 0;
double lon = 0;

float max_alt = 0.0;
float timerArmagedon = 0.0;

void setup(){
  Serial.begin(9600);
  GNSS.begin(115200);

  Wire.begin();

  if (!bmp.begin(0x76)) {
    Serial.println("no bmp");
  }
  Serial.println("bmp init");

  mpu.initialize();
  mpu.dmpInitialize();
  mpu.setDMPEnabled(true);


  
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(869.4E6)) {
    Serial.println("no lora");
  }

  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(10);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0xF3);

  Serial.println("lora init");
  
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A17, INPUT);

  pinMode(buzzer, OUTPUT);
  
  SD.begin(BUILTIN_SDCARD);

  int fileIndex = 0;
  while (true) {
    sprintf(filename, "log%d.txt", fileIndex);
    if (!SD.exists(filename)) break;
    fileIndex++;
  }

  
}

double ConvertTemp(double x){
  return (((((-1.4078e-12 * x + 3.8668e-09) * x - 4.2095e-06) * x + 2.2864e-03) * x - 7.2374e-01) * x + 1.5920e+02);
}

void loop(){
  if(seaLevelPressure == 0.0)
  {
    seaLevelPressure = bmp.readPressure();
  }
  while(GNSS.available())
  {
    char c = GNSS.read();
    gps.encode(c);
  }
    
  
  float pressure = bmp.readPressure();

  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }
  double readNtc1 = analogRead(A8);
  double readNtc2 = analogRead(A9);
  double readNtc3 = analogRead(A17);

  double ntc1 = ConvertTemp(readNtc1);
  double ntc2 = ConvertTemp(readNtc2);
  double ntc3 = ConvertTemp(readNtc3);
  
  float temp = (ntc1 + ntc3) / 2.0;
  

  float altitude = bmp.readAltitude(seaLevelPressure);
  max_alt = max(max_alt, altitude);

  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);

  if(altitude <= max_alt - 500)
  {
    digitalWrite(buzzer, HIGH);
  }

  if(millis() >= timerArmagedon){
  timerArmagedon = millis() + 700;
  int16_t tempRaw = temp * 10;
  int32_t latRaw  = lat * 100000;
  int32_t lonRaw  = lon * 100000;
  int32_t presRaw = pressure;
  int16_t qW = q.w * 100;
  int16_t qX = q.x * 100;
  int16_t qY = q.y * 100;
  int16_t qZ = q.z * 100;

  byte packet[22];
  
  packet[0] = (tempRaw >> 8) & 0xFF;
  packet[1] = tempRaw & 0xFF;
  packet[2] = (latRaw >> 24) & 0xFF;
  packet[3] = (latRaw >> 16) & 0xFF;
  packet[4] = (latRaw >> 8)  & 0xFF;
  packet[5] = latRaw & 0xFF;
  packet[6] = (lonRaw >> 24) & 0xFF;
  packet[7] = (lonRaw >> 16) & 0xFF;
  packet[8] = (lonRaw >> 8)  & 0xFF;
  packet[9] = lonRaw & 0xFF;
 
  
  packet[10] = presRaw >> 24;
  packet[11] = presRaw >> 16;
  packet[12] = presRaw >> 8;
  packet[13] = presRaw;

  packet[14] = qW >> 8;
  packet[15] = qW;

  packet[16] = qX >> 8;
  packet[17] = qX;

  packet[18] = qY >> 8;
  packet[19] = qY;

  packet[20] = qZ >> 8;
  packet[21] = qZ;

  LoRa.beginPacket();
  LoRa.write(packet, 22);

  LoRa.endPacket();

  f = SD.open(filename, FILE_WRITE);
  f.print(counter); f.print(' ');
  f.print(temp);   f.print(' ');
  f.print(pressure);  f.print(' ');
  f.print(lon, 6); f.print(' ');
  f.print(lat, 6); f.print(' ');
  f.print(q.w); f.print(' ');
  f.print(q.x); f.print(' ');
  f.print(q.y); f.print(' ');
  f.println(q.z);
  f.close();

  }
  counter++;
}