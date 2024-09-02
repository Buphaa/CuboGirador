#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Inicializar el MPU6050
Adafruit_MPU6050 mpu;

// Variables para almacenar los valores de calibración
float gyroXOffset, gyroYOffset, gyroZOffset;
float accelXOffset, accelYOffset, accelZOffset;

void setup() {
  Serial.begin(115200);

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

  // Calibrar el sensor
  calibrateMPU6050();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Aplicar la corrección de desviación (offset)
  float correctedGyroX = g.gyro.x - gyroXOffset;
  float correctedGyroY = g.gyro.y - gyroYOffset;
  float correctedGyroZ = g.gyro.z - gyroZOffset;

  float correctedAccelX = a.acceleration.x - accelXOffset;
  float correctedAccelY = a.acceleration.y - accelYOffset;
  float correctedAccelZ = a.acceleration.z - accelZOffset;

  // Usar los valores corregidos en tu aplicación
  Serial.print("GyroX: "); Serial.print(correctedGyroX);
  Serial.print(" GyroY: "); Serial.print(correctedGyroY);
  Serial.print(" GyroZ: "); Serial.print(correctedGyroZ);
  Serial.print(" AccelX: "); Serial.print(correctedAccelX);
  Serial.print(" AccelY: "); Serial.print(correctedAccelY);
  Serial.print(" AccelZ: "); Serial.println(correctedAccelZ);

  delay(100);
}

void calibrateMPU6050() {
  const int samples = 1000;
  float gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;
  float accelXSum = 0, accelYSum = 0, accelZSum = 0;

  Serial.println("Calibrando... Por favor, mantén el sensor quieto.");

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    gyroXSum += g.gyro.x;
    gyroYSum += g.gyro.y;
    gyroZSum += g.gyro.z;

    accelXSum += a.acceleration.x;
    accelYSum += a.acceleration.y;
    accelZSum += a.acceleration.z;

    delay(3);
  }

  // Calcular los promedios
  gyroXOffset = gyroXSum / samples;
  gyroYOffset = gyroYSum / samples;
  gyroZOffset = gyroZSum / samples;

  accelXOffset = accelXSum / samples;
  accelYOffset = accelYSum / samples;
  accelZOffset = accelZSum / samples;

  Serial.println("Calibración completa.");
}
