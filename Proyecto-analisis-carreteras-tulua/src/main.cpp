#include <Arduino.h>             //libreria general
#include <Wire.h>                //i2c protocol
#include <Adafruit_MPU6050.h>    // Para el acelerómetro MPU6050
#include <Adafruit_SSD1306.h>    // Para la pantalla OLED
#include <Adafruit_GFX.h>        // gráfica para la pantalla OLED

#include <TinyGPSPlus.h>         // para el modulo gps
#include <SoftwareSerial.h>      // para ampliar los tx y rx en esp32

// Definición de parámetros
#define SCREEN_WIDTH 128    //tamaño de pantall ancho
#define SCREEN_HEIGHT 64    //taaño de pantalla alto
#define OLED_RESET    -1    // -1 oled = no reset
int buttonPin1 = 16;  // activar switch 1 ronda
int buttonPin2 = 17;  // activar switch 2 ronda
int buttonPin3 = 5;   // para activar siwtch 3 ronda
int buttonPin4 = 18;  //  para enviar los datos, aun no se sabe donde

SoftwareSerial ss(12, 13); // RX, TX (ajusta según tus conexiones)   etan inversos
TinyGPSPlus gps;



//datos para detectar aceleracion brusca
const float accelerationThreshold = 2.5;  // Threshold para detectar aceleración brusca en m/s²
const float gyroThreshold = 50;           // Threshold para giros bruscos en grados por segundo
const float maxGyroAngle = 60.0;          // Máximo ángulo permitido antes de llegar a 10


// Inicializa el MPU6050 y la pantalla OLED
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



void setup() {
    //inicio de pistas para switch
    pinMode(buttonPin1, INPUT_PULLDOWN); //1 ronda
    pinMode(buttonPin2, INPUT_PULLDOWN); //2 ronda
    pinMode(buttonPin3, INPUT_PULLDOWN); //3 ronda
    pinMode(buttonPin4, INPUT_PULLDOWN); //subir datos

    // Inicializar comunicación serial para depuración
    Serial.begin(115200);  
    ss.begin(9600);

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

void leer_datos(int buttonPin){
    
    while(digitalRead(buttonPin) == HIGH){

    int pantalla = 0;
    // Leer aceleración y giroscopio.

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Calcula la magnitud del vector de aceleración
    float accelMagnitude = sqrt(sq(a.acceleration.x) + sq(a.acceleration.y) + sq(a.acceleration.z));
    
    // Calcula la magnitud del vector de giro
    float gyroMagnitude = sqrt(sq(g.gyro.x) + sq(g.gyro.y) + sq(g.gyro.z)) * 57.2958;  // Convertir a grados/s

    // Determina la escala de perturbación
    int movimientoBrusco = calcularEscalaDeMovimiento(accelMagnitude, gyroMagnitude);

    // Imprime la escala de 0 a 10
    display.println("Perturbacion: ");
    display.println(movimientoBrusco);

    Serial.print(movimientoBrusco);
    

    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    Serial.print("Latitud: ");
    Serial.println(latitude, 6);
    Serial.print("Longitud: ");
    Serial.println(longitude, 6);
    Serial.print("Número de satélites: ");
    Serial.println(gps.satellites.value());
    Serial.print("HDOP: ");
    Serial.println(gps.hdop.value());
    Serial.print("Velocidad (km/h): ");
    Serial.println(gps.speed.kmph());
    Serial.println("-------------------------");

    // Reiniciar el contador de tiempo
    display.setCursor(0,0);
    display.print("latitud: ");
    display.println(latitude,6);
    display.setCursor(0,10);
    display.print("longitud: ");
    display.println(longitude,6);
    display.setCursor(0,20);
    display.print("Nro de satelites: ");
    display.println(gps.satellites.value());
    display.display();

 
    display.clearDisplay();
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
    display.println("Iniciando Ronda 1 en.. ");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin1);
}

void ronda2(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Iniciando Ronda 2 en...");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin2);
}

void ronda3(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Iniciando Ronda 3 en...");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin3);
}

void loop() {
  
    // Si el botón está presionado o el dipswitch activado (estado HIGH)
     if (digitalRead(buttonPin1) == HIGH) {
     // Ejecutar la operación deseada
     ronda1();
     }else if (digitalRead(buttonPin2) == HIGH)
     {
     ronda2();
     }else if(digitalRead(buttonPin3) == HIGH)
     {
     ronda3();
     }else if(digitalRead(buttonPin4) == HIGH)
     {
    display.setCursor(0, 0);
    display.clearDisplay();
    display.print("subiendo datos >");
    for (int i = 0; i < 10; i++)
    {
    display.print(">>");
    display.display();
    delay(1000);
    }
    
    }else{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("-----En espera-----");
    display.setCursor(0, 10);
    display.print("-----iniciar?-----");
    display.setCursor(0, 20);
    display.print("switch 1-3/ronda 1-3"); 
    display.setCursor(0, 30);
    display.print("switch 4/subir data"); 
    display.display();
    delay(100);
    }
     

}



