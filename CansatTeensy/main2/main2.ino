#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>
#include <SD.h>

#define GNSS Serial1
#define ntc1 A7
#define ntc2 A8
#define ntc3 A17
#define buzzer 38

#define SERIESRESISTOR 10000  // resistor in voltage divider (ohms)
#define NOMINAL_RESISTANCE 10000  // NTC resistance at 25°C
#define NOMINAL_TEMP 25
#define BCOEFFICIENT 3950 

TinyGPSPlus gps;
Adafruit_BMP280 bmp;
int counter = 0;
float seaLevelPressure = 0.0;

File f;
char filename[15];

//// TO DO servo buzzer

float adcToTemp(int adc) {
  float resistance = SERIESRESISTOR / (1023.0 / adc - 1.0);
  float steinhart = log(resistance / NOMINAL_RESISTANCE) / BCOEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMP + 273.15);
  return (1.0 / steinhart) - 273.15;
}

void setup() {
  Serial.begin(115200);
  GNSS.begin(115200);

  pinMode(buzzer, OUTPUT);

  SD.begin(BUILTIN_SDCARD);

  int fileIndex = 0;
  while (true) {
    sprintf(filename, "log%d.txt", fileIndex);
    if (!SD.exists(filename)) break;
    fileIndex++;
  }
  
}

void loop() {
  if(seaLevelPressure == 0.0)
  {
    seaLevelPressure = bmp.readPressure();
    float max_alt = bmp.readAltitude();
  }

  while (GNSS.available()){
    gps.encode(GNSS.read());
  }

  float pressure = bmp.readPressure();
  float longitude = gps.location.lng();
  float latitude  = gps.location.lat(); 

  float avgTemp = (adcToTemp(analogRead(ntc1)) +
                   adcToTemp(analogRead(ntc2)) +
                   adcToTemp(analogRead(ntc3))) / 3.0f;

  float altitude = bmp.readAltitude(seaLevelPressure);

  max_alt = max(max_alt, altitude);

  Serial.print(counter++); Serial.print(' ');
  Serial.print(avgTemp);   Serial.print(' ');
  Serial.print(pressure);  Serial.print(' ');
  Serial.print(longitude); Serial.print(' ');
  Serial.println(latitude);

  f = SD.open(filename, FILE_WRITE);
  f.print(counter); f.print(' ');
  f.print(avgTemp);   f.print(' ');
  f.print(pressure);  f.print(' ');
  f.print(longitude); f.print(' ');
  f.println(latitude);
  f.close();

  if(altitude == max_alt - 500)
  {
    tone(buzzer, 1000);
  }

  delay(1000);
}