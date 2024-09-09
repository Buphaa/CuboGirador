#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>  // Librería para la pantalla integrada en la ESP32 TTGO

// Inicializar la pantalla
TFT_eSPI tft = TFT_eSPI();

// Inicializar el MPU6050
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  // Iniciar la pantalla
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);  // Comenzar con la pantalla en negro

  // Iniciar el MPU6050
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un MPU6050");
    while (1);
  }
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print(a.acceleration.y);
  Serial.print("-");
  Serial.print(a.acceleration.z);
  Serial.print("-");
  Serial.println(a.acceleration.x);

  // Mapear los valores de aceleración a un rango de 0 a 255
  int red = map(abs(a.acceleration.y), 0, 10, 0, 255);
  int green = map(abs(a.acceleration.z), 0, 10, 0, 255);
  int blue = map(abs(a.acceleration.x), 0, 10, 0, 255);

  // Asegurarse de que los valores estén dentro del rango válido
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  // Convertir los valores RGB a un formato de 16 bits
  uint16_t color = tft.color565(red, green, blue);

  // Cambiar el color de la pantalla
  tft.fillScreen(color);

  delay(100); // Espera un poco antes de la siguiente lectura
}
