#include <SD.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <TinyGPS++.h>

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
  while (GNSS.available()){
    gps.encode(GNSS.read());
  }

  float pressure = bmp.readPressure();
  float longitude = gps.location.lng();
  float latitude  = gps.location.lat(); 

  float avgTemp = (adcToTemp(analogRead(ntc1)) +
                   adcToTemp(analogRead(ntc2)) +
                   adcToTemp(analogRead(ntc3))) / 3.0f;

  float speed = gps.speed.mps();
  float t_vel = 28;

  Serial.print(counter++); Serial.print(' ');
  Serial.print(avgTemp);   Serial.print(' ');
  Serial.print(pressure);  Serial.print(' ');
  Serial.print(longitude); Serial.print(' ');
  Serial.println(latitude);

  f = SD.open(filename, FILE_WRITE);
  f.print(counter++); f.print(' ');
  f.print(avgTemp);   f.print(' ');
  f.print(pressure);  f.print(' ');
  f.print(longitude); f.print(' ');
  f.println(latitude);
  f.close();

  if(speed = t_vel)  /// ajunge la terminal velocity
  {
    tone(buzzer, 1000);
    delay(500);
    noTone(buzzer);
    delay(500);

    /// release parachute
  }

  delay(1000);
}


/*
const uint8_t MPU = 0x68;
float qw=1, qx=0, qy=0, qz=0;
unsigned long t;



Wire.begin();

  if (!bmp.begin(0x76)) {  // try 0x77 if this fails
    Serial.println("BMP280 not found!");
    while(1);
  }

  Wire.beginTransmission(MPU);
  Wire.write(0x6B); Wire.write(0);
  Wire.endTransmission(true);

  t = micros();



 Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  float gx = (int16_t)(Wire.read()<<8|Wire.read()) / 7424.f;
  float gy = (int16_t)(Wire.read()<<8|Wire.read()) / 7424.f;
  float gz = (int16_t)(Wire.read()<<8|Wire.read()) / 7424.f;

  float dt = (micros()-t)/1e6f; t = micros();

  float dw=-qx*gx-qy*gy-qz*gz, dx=qw*gx+qy*gz-qz*gy;
  float dy=qw*gy-qx*gz+qz*gx,  dz=qw*gz+qx*gy-qy*gx;

  qw+=.5*dw*dt; qx+=.5*dx*dt; qy+=.5*dy*dt; qz+=.5*dz*dt;

  float n=sqrt(qw*qw+qx*qx+qy*qy+qz*qz);
  qw/=n; qx/=n; qy/=n; qz/=n;

  Serial.print(qw,4); Serial.print(' ');
  Serial.print(qx,4); Serial.print(' ');
  Serial.print(qy,4); Serial.print(' ');
  Serial.println(qz,4);
  Serial.println();

*/