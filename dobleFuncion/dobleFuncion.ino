#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
Adafruit_MPU6050 mpu;

float initialX, initialY, initialZ;
bool calibrando = true;
bool modoSecuencia = false; 
const int botonPin = 15;  
int ultimoEstadoBoton = HIGH; 

void setup() {
  Serial.begin(115200);

  pinMode(botonPin, INPUT_PULLUP); 

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
  // Leer el estado del botón
  int estadoBoton = digitalRead(botonPin);
  

  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH) {
    modoSecuencia = !modoSecuencia;  
    delay(200); 
  }
  ultimoEstadoBoton = estadoBoton;

  if (modoSecuencia) {
    mostrarSecuenciaAleatoria();
  } else {
    mostrarColorSensor();
  }

  delay(100);
}

void mostrarColorSensor() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

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
}

void mostrarSecuenciaAleatoria() {
  int red = random(0, 256);
  int green = random(0, 256);
  int blue = random(0, 256);

  uint16_t color = tft.color565(red, green, blue);
  tft.fillScreen(color);
  delay(500);  
}
