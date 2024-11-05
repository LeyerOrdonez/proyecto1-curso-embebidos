#include <Arduino.h>
#include <Wire.h>                //i2c protocol
#include <Adafruit_MPU6050.h>    // Para el acelerómetro MPU6050
#include <Adafruit_SSD1306.h>    // Para la pantalla OLED
#include <Adafruit_GFX.h>        // gráfica para la pantalla OLED


// Definición de parámetros
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1    // -1 oled = no reset
int buttonPin1 = 16;  //switch 1 ronda
int buttonPin2 = 17;   //switch 2 ronda
int buttonPin3 = 5;  // siwtch 3 ronda
int buttonPin4 = 18;  // subirdata


const float accelerationThreshold = 2.5;  // Threshold para detectar aceleración brusca en m/s²
const float gyroThreshold = 50;           // Threshold para giros bruscos en grados por segundo
const float maxGyroAngle = 60.0;          // Máximo ángulo permitido antes de llegar a 10



// Inicializa el MPU6050 y la pantalla OLED
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    //inicio de pistas
    pinMode(buttonPin1, INPUT_PULLDOWN); //1 ronda
    pinMode(buttonPin2, INPUT_PULLDOWN); //2 ronda
    pinMode(buttonPin3, INPUT_PULLDOWN); //3 ronda
    pinMode(buttonPin4, INPUT_PULLDOWN); //3 ronda

    // Inicializar comunicación serial para depuración
    Serial.begin(115200);

    // Inicializar el MPU6050
    if (!mpu.begin()) {
        Serial.println("Error al inicializar el MPU6050.");
        while (1); // Bloquea el código si no se inicializa (peligroso)
    }
    Serial.println("MPU6050 inicializado correctamente.");


    // Inicializar la pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección 0x3C para la pantalla OLED
        Serial.println("Error al inicializar la pantalla OLED.");
        while (1); // Bloquea el código si no se inicializa (peligroso)
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    
}


void leer_datos(int buttonPin){
    
    while(digitalRead(buttonPin) == HIGH){

    int pantalla = 0;
    // Leer aceleración y giroscopio.

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Limpiar la pantalla antes de mostrar nuevos datos
    display.clearDisplay();

    // Mostrar los datos del acelerómetro en la pantalla OLED
    display.setCursor(0, pantalla);
    display.print("Ax: ");
    display.print(a.acceleration.x);
    display.print("  Gx: ");
    display.print(g.gyro.x);

    display.setCursor(0, pantalla+=10);
    display.print("Ay: ");
    display.print(a.acceleration.y);
    display.print("  Gy: ");
    display.print(g.gyro.y);
    
    display.setCursor(0, pantalla+=10);
    display.print("Az: ");
    display.print(a.acceleration.z);
    display.print("  Gz: ");
    display.print(g.gyro.z);


    display.display();

    
    delay(100);
    }

    display.clearDisplay();
    display.setCursor(0,0);
    display.print("terminando ronda");
    display.display();
    delay(2000);

}


void ronda1(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Iniciando Ronda 1 en...");
    display.print(" 3");
    display.display();
    delay(1000);
    display.print(" 2");
    display.display();
    delay(1000);
    display.print(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin1);
}

void ronda2(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Iniciando Ronda 2 en...");
    display.print(" 3");
    display.display();
    delay(1000);
    display.print(" 2");
    display.display();
    delay(1000);
    display.print(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin2);
}

void ronda3(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Iniciando Ronda 3 en...");
    display.print(" 3");
    display.display();
    delay(1000);
    display.print(" 2");
    display.display();
    delay(1000);
    display.print(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin3);
}


int calcularEscalaDeMovimiento(float accelMagnitude, float gyroMagnitude) {
    int escala = 0;

    // Si la aceleración es cercana a la gravedad (reposo)
    if (fabs(accelMagnitude - 9.8) < 0.2 && gyroMagnitude < 1) {
        return 0;  // Sin perturbación
    }

    // Aceleración brusca
    if (accelMagnitude > accelerationThreshold) {
        escala += map(accelMagnitude, accelerationThreshold, 15.0, 1, 5);  // Ajustar los límites según sea necesario
    }

    // Giros bruscos (medidos en grados por segundo)
    if (gyroMagnitude > gyroThreshold) {
        escala += map(gyroMagnitude, gyroThreshold, 500.0, 1, 5);  // Ajustar los límites
    }

    // Limita la escala a un máximo de 10
    if (escala > 10) {
        escala = 10;
    }

    return escala;
}


void loop() {
  
    // // Si el botón está presionado o el dipswitch activado (estado HIGH)
    //  if (digitalRead(buttonPin1) == HIGH) {
    //  // Ejecutar la operación deseada
    //  ronda1();
    //  }else if (digitalRead(buttonPin2) == HIGH)
    //  {
    //  ronda2();
    //  }else if(digitalRead(buttonPin3) == HIGH)
    //  {
    //  ronda3();
    //  }else if(digitalRead(buttonPin4) == HIGH)
    //  {
    // display.setCursor(0, 0);
    // display.clearDisplay();
    // display.print("subiendo datos >");
    // for (int i = 0; i < 10; i++)
    // {
    // display.print(">>");
    // display.display();
    // delay(1000);
    // }
    
    //  }else{
    // display.clearDisplay();
    // display.setCursor(0, 0);
    // display.print("En espera de la siguiente orden...");
    // display.setCursor(0, 20);
    // display.print("iniciar?"); 
    // display.display();
    // delay(100);
    //  }
     
sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Calcula la magnitud del vector de aceleración
    float accelMagnitude = sqrt(sq(a.acceleration.x) + sq(a.acceleration.y) + sq(a.acceleration.z));
    
    // Calcula la magnitud del vector de giro
    float gyroMagnitude = sqrt(sq(g.gyro.x) + sq(g.gyro.y) + sq(g.gyro.z)) * 57.2958;  // Convertir a grados/s

    // Determina la escala de perturbación
    int movimientoBrusco = calcularEscalaDeMovimiento(accelMagnitude, gyroMagnitude);

    // Imprime la escala de 0 a 10
    display.setCursor(0,0);
    display.print("Perturbacion: ");
    display.println(movimientoBrusco);

    Serial.print(movimientoBrusco);
    display.display();

    delay(100);
    display.clearDisplay();

}



