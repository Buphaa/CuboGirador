#include <Wire.h>

#include <Adafruit_GFX.h>

#include <Adafruit_MPU6050.h>

#include <Adafruit_Sensor.h>

#include <TFT_eSPI.h>  // Librería para la pantalla integrada en la ESP32 TTGO
 
// Inicializar la pantalla

TFT_eSPI tft = TFT_eSPI();
 
// Inicializar el MPU6050

Adafruit_MPU6050 mpu;
 
// Definir algunos colores personalizados en formato RGB565

#define TFT_ORANGE  0xFDA0

#define TFT_PINK    0xF81F

#define TFT_PURPLE  0x780F

#define TFT_MAGENTA 0xF81F

#define TFT_VIOLET  0x801F

#define TFT_NAVY    0x000F

#define TFT_TEAL    0x0438

#define TFT_AQUA    0x07FF

#define TFT_LIME    0x07E0

#define TFT_OLIVE   0x8400

#define TFT_MAROON  0x8000

#define TFT_SILVER  0xC618

#define TFT_GRAY    0x8410
 
// Función para mapear un ángulo a un color

uint16_t getColorForAngle(float angle) {

  // Asegurarse de que el ángulo esté entre 0 y 360

  angle = fmod(angle, 360);

  if (angle < 0) angle += 360;
 
  if (angle < 18) return TFT_RED;

  else if (angle < 36) return TFT_ORANGE;

  else if (angle < 54) return TFT_YELLOW;

  else if (angle < 72) return TFT_GREEN;

  else if (angle < 90) return TFT_CYAN;

  else if (angle < 108) return TFT_BLUE;

  else if (angle < 126) return TFT_PURPLE;

  else if (angle < 144) return TFT_PINK;

  else if (angle < 162) return TFT_MAGENTA;

  else if (angle < 180) return TFT_VIOLET;

  else if (angle < 198) return TFT_NAVY;

  else if (angle < 216) return TFT_TEAL;

  else if (angle < 234) return TFT_AQUA;

  else if (angle < 252) return TFT_LIME;

  else if (angle < 270) return TFT_OLIVE;

  else if (angle < 288) return TFT_MAROON;

  else if (angle < 306) return TFT_SILVER;

  else if (angle < 324) return TFT_GRAY;

  else if (angle < 342) return TFT_WHITE;

  else return TFT_BLACK;

}
 
void setup() {

  // Iniciar la comunicación serial para depuración

  Serial.begin(115200);
 
  // Iniciar la pantalla

  tft.init();

  tft.setRotation(1);

  tft.fillScreen(TFT_WHITE);  // Comenzar con la pantalla en blanco
 
  // Iniciar el MPU6050

  if (!mpu.begin()) {

    Serial.println("No se pudo encontrar un MPU6050, revisa la conexión!");

    while (1);

  }

  Serial.println("MPU6050 encontrado!");
 
  // Configurar el rango de aceleración y giroscopio

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

}
 
void loop() {

  // Leer los valores del sensor

  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);
 
  // Calcular el ángulo del eje X e Y

  float angleX = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * 180 / PI;

  float angleY = atan2(a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;
 
  // Obtener el color para cada eje

  uint16_t colorX = getColorForAngle(angleX);

  uint16_t colorY = getColorForAngle(angleY);
 
  // Mostrar el color basado en el ángulo más dominante

  if (abs(a.acceleration.x) > abs(a.acceleration.y)) {

    tft.fillScreen(colorX);  // Cambiar color según el ángulo en el eje X

  } else {

    tft.fillScreen(colorY);  // Cambiar color según el ángulo en el eje Y

  }
 
  // Esperar un poco antes de la siguiente lectura

  delay(100);

}