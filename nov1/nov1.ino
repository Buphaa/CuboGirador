#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <BluetoothSerial.h>

// Definir la cantidad de LEDs y el pin
#define LED_PIN 5       // Pin donde está conectada la tira Neopixel
#define NUM_LEDS 12     // Número de LEDs en la tira

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Inicializar el MPU6050 y Bluetooth
Adafruit_MPU6050 mpu;
BluetoothSerial SerialBT;

// Variables para el color recibido vía Bluetooth
int targetRed = 0, targetGreen = 0, targetBlue = 0;
float initialX, initialY, initialZ;

bool calibrando = true;
bool modoFiesta = false;

int red = 255, green = 0, blue = 0;
int breathSpeed = 22; // Declaraciones de funciones

bool parseColorData(String data);
bool compararColores();
void setLedColor(int r, int g, int b);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Cubito");  // Nombre Bluetooth
  Serial.println("Bluetooth iniciado");

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
  // Si hay datos disponibles por Bluetooth, leer el color
  if (SerialBT.available()) {
    String colorData = SerialBT.readStringUntil('\n');
    if (parseColorData(colorData)) {
      Serial.println("Color recibido: " + colorData);
    }
  }

  if (modoFiesta) {
    mostrarModoFiesta();
  } else {
    mostrarColorSensor();
  }

  // Comparar colores entre MPU6050 y el recibido por Bluetooth
  if (compararColores()) {
    Serial.println("¡Colores iguales! ¡Ganaste!");
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

  red = map(a.acceleration.y - initialY, -5, 5, 0, 255);
  green = map(a.acceleration.z - initialZ, -5, 5, 0, 255);
  blue = map(a.acceleration.x - initialX, -5, 5, 0, 255);
  
  // Asegurarse de que los valores nunca sean 0
  red = max(red, 5);
  green = max(green, 5);
  blue = max(blue, 5);

  setLedColor(red, green, blue);
}


// Función para el efecto de respiración en la tira LED
void mostrarModoFiesta() {
  breathSpeed = 5;  // Ajuste de velocidad de cambio

  if (red > 0 && blue == 0) {
    red -= breathSpeed;
    green += breathSpeed;
  } else if (green > 0 && red == 0) {
    green -= breathSpeed;
    blue += breathSpeed;
  } else if (blue > 0 && green == 0) {
    blue -= breathSpeed;
    red += breathSpeed;
  }

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);
  setLedColor(red, green, blue);
}

// Función para establecer el color en la tira LED
void setLedColor(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();

  // Enviar el color al script de Python
  SerialBT.printf("%d,%d,%d\n", r, g, b);
}

// Función para analizar el color recibido vía Bluetooth
bool parseColorData(String data) {
  int commaIndex1 = data.indexOf(',');
  int commaIndex2 = data.indexOf(',', commaIndex1 + 1);
  if (commaIndex1 == -1 || commaIndex2 == -1) return false;
  targetRed = data.substring(0, commaIndex1).toInt();
  targetGreen = data.substring(commaIndex1 + 1, commaIndex2).toInt();
  targetBlue = data.substring(commaIndex2 + 1).toInt();
  return true;
}

// Función para comparar los colores del sensor y el Bluetooth
bool compararColores() {
  int diffR = abs(red - targetRed);
  int diffG = abs(green - targetGreen);
  int diffB = abs(blue - targetBlue);
  return (diffR < 20 && diffG < 20 && diffB < 20);
}
