#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>  

TFT_eSPI tft = TFT_eSPI();

// Inicializar el MPU6050
Adafruit_MPU6050 mpu;

float initialX, initialY, initialZ;
bool calibrando = true;

void setup() {
  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);  

 
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un MPU6050");
    while (1);
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  initialX = a.acceleration.x;
  initialY = a.acceleration.y;
  initialZ = a.acceleration.z;
  calibrando = false;
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print(a.acceleration.y);
  Serial.print("-");
  Serial.print(a.acceleration.z);
  Serial.print("-");
  Serial.println(a.acceleration.x);

  if (!calibrando && abs(a.acceleration.x - initialX) < 0.1 && abs(a.acceleration.y - initialY) < 0.1 && abs(a.acceleration.z - initialZ) < 0.1) {
    tft.fillScreen(TFT_BLACK); 
  } else {
   
    int red = map(abs(a.acceleration.y - initialY), 0, 10, 0, 255);
    int green = map(abs(a.acceleration.z - initialZ), 0, 10, 0, 255);
    int blue = map(abs(a.acceleration.x - initialX), 0, 10, 0, 255);

    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);

    uint16_t color = tft.color565(red, green, blue);

    tft.fillScreen(color);
  }

  delay(100);
}
