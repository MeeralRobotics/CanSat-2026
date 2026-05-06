#include <SPI.h>
#include <LoRa.h>
#include <SD.h>

float seaLevelPressure = 0.0;

File f;
char filename[15];


void setup() {
  Serial.begin(9600);
  LoRa.begin(869.4E6);




  SD.begin(BUILTIN_SDCARD);

  int fileIndex = 0;
  while (true) {
    sprintf(filename, "log%d.txt", fileIndex);
    if (!SD.exists(filename)) break;
    fileIndex++;
  }
  

  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(10);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0xF3);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  //Serial.println(packetSize);
  if (packetSize == 22) {
    byte packet[22];

    for (int i = 0; i < 22; i++) {
      packet[i] = LoRa.read();
    }


    int16_t tempRaw = (packet[0] << 8) | packet[1];

    int32_t latRaw = (packet[2] << 24) |
                     (packet[3] << 16) |
                     (packet[4] << 8)  |
                     packet[5];

    int32_t lonRaw = (packet[6] << 24) |
                     (packet[7] << 16) |
                     (packet[8] << 8)  |
                     packet[9];

    int32_t presRaw = (packet[10] << 24) |
                  (packet[11] << 16) |
                  (packet[12] << 8)  |
                  packet[13];


    float qW_raw = ((int16_t)((packet[14] << 8) | packet[15])) / 100.0;
    float qX_raw = ((int16_t)((packet[16] << 8) | packet[17])) / 100.0;
    float qY_raw = ((int16_t)((packet[18] << 8) | packet[19])) / 100.0;
    float qZ_raw = ((int16_t)((packet[20] << 8) | packet[21])) / 100.0;

    float temp = tempRaw / 10.0;
    float lat  = latRaw / 100000.0;
    float lon  = lonRaw / 100000.0;
    float pressure = presRaw;

    float altitude;
    pressure /= 100;
  
  if(seaLevelPressure == 0.0)
  {
    seaLevelPressure = pressure;
  }

    altitude = 44330 * (1.0 - pow(pressure / seaLevelPressure, 0.1903));

  
    float qW = qW_raw;
    float qX = qX_raw;
    float qY = qY_raw;
    float qZ = qZ_raw;
    
  f = SD.open(filename, FILE_WRITE);
  //f.print(counter); f.print(' ');
  f.print(temp);   f.print(' ');
  f.print(pressure);  f.print(' ');
  f.print(lon); f.print(' ');
  f.println(lat);
  f.close();
    
    Serial.print(temp);
    Serial.print(",");  
    Serial.print(lat, 6);
    Serial.print(",");  
    Serial.print(lon, 6);
    Serial.print(","); 
    Serial.print(pressure);
    Serial.print(",");
    Serial.print(altitude);
    Serial.print(",");
    Serial.print(qW);
    Serial.print(",");
    Serial.print(qX);
    Serial.print(",");
    Serial.print(qY);
    Serial.print(",");
    Serial.println(qZ);
  }
}