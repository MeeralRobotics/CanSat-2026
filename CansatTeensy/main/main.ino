#include  <Adafruit_BMP280.h>
#include  <Adafruit_MPU6050.h>
#include <Wire.h>

Adafruit_BMP280 bmp; 
Adafruit_MPU6050 mpu;


float defaultPressure = 0.0f;
sensors_event_t g;


void setup()  {
  Serial.begin(9600);
  
  Wire.begin();
  //Serial.begin(9600);
  Serial.println("Scanning I2C bus...");

  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(i, HEX);
    }
  }




 // if  (!bmp.begin(0x76)) {
   // Serial.println("No bmp");
    //while (1);
  //}

  if(!mpu.begin(0x68)){
    Serial.println("No mpu");
    while (1);
  }
  
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,  
                  Adafruit_BMP280::SAMPLING_X2,     
                  Adafruit_BMP280::SAMPLING_X8,   
                  Adafruit_BMP280::FILTER_X4,    
                  Adafruit_BMP280::STANDBY_MS_1);  

  defaultPressure = bmp.readPressure()/100;

  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
}

void loop() {


  // bmp280
    Serial.print("Temperature  = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure()/100);
    Serial.println("  hPa");

    Serial.print("Approx altitude = ");
    Serial.print(bmp.readAltitude(defaultPressure)); 
    Serial.println("  m");                    

    Serial.println();


  // mpu6050
    mpu.getEvent(NULL, &g, NULL);
    Serial.print(g.gyro.x);
    Serial.print("  ");
    Serial.print(g.gyro.y);
    Serial.print("  ");
    Serial.print(g.gyro.z);
    Serial.println();


    delay(2000);
}
