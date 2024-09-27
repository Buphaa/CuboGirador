#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>  

TFT_eSPI tft = TFT_eSPI();
Adafruit_MPU6050 mpu;

float initialX, initialY, initialZ;
bool calibrando = true;
bool modoSecuencia = false; // Estado del modo (sensor o secuencia de colores)
const int botonPin = 15;  // Pin del botón, ajusta según tu conexión
int ultimoEstadoBoton = HIGH; // Estado previo del botón

// Variables para el efecto de respiración
int red = 255, green = 0, blue = 0;  // Comienza con rojo
int breathSpeed = 22; // Controla la velocidad del efecto de respiración

void setup() {
  Serial.begin(115200);
  
  pinMode(botonPin, INPUT_PULLUP); // Configurar el pin del botón como entrada
  
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

  // Verificar si el botón ha sido presionado
  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH) {
    modoSecuencia = !modoSecuencia;  // Alternar el modo
    delay(200); // Pequeña pausa para evitar rebotes
  }
  ultimoEstadoBoton = estadoBoton;

  if (modoSecuencia) {
    mostrarEfectoRespiracion(); // Mostrar efecto de respiración
  } else {
    mostrarColorSensor();  // Mostrar el color basado en el sensor
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

// Función para mostrar el efecto de respiración
void mostrarEfectoRespiracion() {
  // Transiciones suaves entre colores
  if (red > 0 && blue == 0) {
    red -= breathSpeed;  // Reducir el rojo
    green += breathSpeed;  // Aumentar el verde
  }
  if (green > 0 && red == 0) {
    green -= breathSpeed;  // Reducir el verde
    blue += breathSpeed;  // Aumentar el azul
  }
  if (blue > 0 && green == 0) {
    blue -= breathSpeed;  // Reducir el azul
    red += breathSpeed;  // Aumentar el rojo
  }

  // Asegurarse de que los valores estén en el rango válido
  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  // Crear el color con los valores de "respiración"
  uint16_t color = tft.color565(red, green, blue);

  // Mostrar el color en pantalla
  tft.fillScreen(color);

  // Controlar la velocidad de respiración
  delay(30);  // Ajusta este valor para controlar la rapidez del efecto
}
