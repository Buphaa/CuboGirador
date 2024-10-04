#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

// Definir la cantidad de LEDs y el pin
#define LED_PIN 23     // Pin donde está conectada la tira Neopixel
#define NUM_LEDS 6   // Número de LEDs en la tira

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Inicializar el MPU6050
Adafruit_MPU6050 mpu;

float initialX, initialY, initialZ;
bool calibrando = true;
bool modoSecuencia = false; 
const int botonPin = 4;  
int ultimoEstadoBoton = HIGH; 

int red = 255, green = 0, blue = 0;  
int breathSpeed = 22; 

void setup() {
  Serial.begin(115200);
  
  pinMode(botonPin, INPUT_PULLUP);
  
  strip.begin();
  strip.show();  // Inicializar todos los LEDs apagados
  
  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un MPU6050");
    while (1);
  }

  // Calibrar valores iniciales del sensor
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  initialX = a.acceleration.x;
  initialY = a.acceleration.y;
  initialZ = a.acceleration.z;
  calibrando = false;

  Serial.println("Sensor calibrado");
}

void loop() {
  
  int estadoBoton = digitalRead(botonPin);

  if (estadoBoton == LOW && ultimoEstadoBoton == HIGH) {
    modoSecuencia = !modoSecuencia;  
    delay(200); 
  }
  ultimoEstadoBoton = estadoBoton;

  if (modoSecuencia) {
    mostrarEfectoRespiracion(); 
  } else {
    mostrarColorSensor();  
  }

  delay(100);
}

// Mostrar el color basado en la aceleración del sensor MPU6050
void mostrarColorSensor() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" Z: ");
  Serial.println(a.acceleration.z);

  if (!calibrando && abs(a.acceleration.x - initialX) < 0.1 && abs(a.acceleration.y - initialY) < 0.1 && abs(a.acceleration.z - initialZ) < 0.1) {
    // Si está en reposo, apagar la tira
    apagarLeds();
  } else {
    // Mapear los valores de los ejes a colores, asegurarse que el rango es correcto
    int red = map(a.acceleration.y - initialY, -10, 10, 0, 255);
    int green = map(a.acceleration.z - initialZ, -10, 10, 0, 255);
    int blue = map(a.acceleration.x - initialX, -10, 10, 0, 255);

    red = constrain(red, 0, 255);
    green = constrain(green, 0, 255);
    blue = constrain(blue, 0, 255);

    // Aplicar el color a todos los LEDs de la tira
    setLedColor(red, green, blue);
  }
}

// Función para el efecto de respiración en la tira LED
void mostrarEfectoRespiracion() {
  if (red > 0 && blue == 0) {
    red -= breathSpeed;  
    green += breathSpeed;  
  }
  if (green > 0 && red == 0) {
    green -= breathSpeed;  
    blue += breathSpeed; 
  }
  if (blue > 0 && green == 0) {
    blue -= breathSpeed; 
    red += breathSpeed;  
  }

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  // Aplicar el color del efecto respiración a los LEDs
  setLedColor(red, green, blue);

  // Controlar la velocidad del efecto respiración
  delay(30);
}

// Función para establecer el color en la tira LED
void setLedColor(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

// Apagar todos los LEDs
void apagarLeds() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);  // Apagar el LED
  }
  strip.show();
}
