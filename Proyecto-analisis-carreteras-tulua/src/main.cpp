#include <Arduino.h>
#include <Wire.h>       //i2c
#include <Adafruit_MPU6050.h>    // Para el acelerómetro MPU6050
#include <Adafruit_SSD1306.h>    // Para la pantalla OLED
#include <Adafruit_GFX.h>        // Librería gráfica para la pantalla OLED

// Definición de parámetros para la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1         // Usar -1 si no se está usando un pin de reset

// Inicializa el MPU6050 y la pantalla OLED
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    // Inicializar comunicación serial para depuración
    Serial.begin(115200);

    // Inicializar el MPU6050
    if (!mpu.begin()) {
        Serial.println("Error al inicializar el MPU6050.");
        while (1); // Bloquea el código si no se inicializa
    }
    Serial.println("MPU6050 inicializado correctamente.");

    // Inicializar la pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección 0x3C para la pantalla OLED
        Serial.println("Error al inicializar la pantalla OLED.");
        while (1); // Bloquea el código si no se inicializa
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void loop() {
    // Leer aceleración del MPU6050
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Limpiar la pantalla antes de mostrar nuevos datos
    display.clearDisplay();

    // Mostrar los datos del acelerómetro en la pantalla OLED
    display.setCursor(0, 0);
    display.print("MPU6050 Acelerometro:");
    
    display.setCursor(0, 10);
    display.print("Ax: ");
    display.print(a.acceleration.x);
    
    display.setCursor(0, 20);
    display.print("Ay: ");
    display.print(a.acceleration.y);
    
    display.setCursor(0, 30);
    display.print("Az: ");
    display.print(a.acceleration.z);

     display.setCursor(0, 40);
    display.print("tmpe: ");
    display.print(temp.temperature);


    // Mostrar los datos en la pantalla
    display.display();

    // Esperar 500 ms antes de actualizar nuevamente
    delay(500);
}

