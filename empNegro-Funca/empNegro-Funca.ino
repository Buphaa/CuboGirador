#include <Adafruit_NeoPixel.h>

// Define el pin donde está conectada la tira LED y el número de LEDs
#define PIN 16  // Cambia al pin que estés utilizando
#define NUM_LEDS 5

// Crea el objeto Neopixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Inicializa la tira de LEDs
  strip.begin();
  strip.show();  // Apaga todos los LEDs al inicio
}

void loop() {
  // Llama a la función que enciende los LEDs en secuencia con diferentes colores
  colorWipe(strip.Color(255, 0, 0), 50); // Rojo
  colorWipe(strip.Color(0, 255, 0), 50); // Verde
  colorWipe(strip.Color(0, 0, 255), 50); // Azul
}

// Función para encender los LEDs con un color y un retardo específico
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);  // Establece el color de cada LED
    strip.show();                   // Actualiza la tira LED
    delay(wait);                    // Espera el tiempo especificado
  }
}
