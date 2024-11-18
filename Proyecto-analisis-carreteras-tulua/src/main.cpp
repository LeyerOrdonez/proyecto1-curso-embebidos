#include <Arduino.h>             //libreria general
#include <Wire.h>                //i2c protocol
#include <Adafruit_MPU6050.h>    // Para el acelerómetro MPU6050
#include <Adafruit_SSD1306.h>    // Para la pantalla OLED
#include <TinyGPSPlus.h>         // para el modulo gps
#include <SoftwareSerial.h>      // para ampliar los tx y rx en esp32

// Definición de parámetros generales
#define SCREEN_WIDTH 128    //tamaño de pantall ancho
#define SCREEN_HEIGHT 64    //taaño de pantalla alto
#define OLED_RESET    -1    // -1 oled = no reset
int buttonPin1 = 16;  // activar switch 1 ronda
int buttonPin2 = 17;  // activar switch 2 ronda
int buttonPin3 = 5;   // para activar siwtch 3 ronda
int buttonPin4 = 18;  //  para enviar los datos, aun no se sabe donde

String globalArray[20];
int i = 0;


//datos para detectar aceleracion brusca
const float accelerationThreshold = 9.8;  // Threshold para detectar aceleración brusca en m/s²


// Inicializa el MPU6050 y pantalla, gps y serial auxiliar
SoftwareSerial ss(12, 13);                // RX, TX | amar-narj
TinyGPSPlus gps;                          // creo la instancia del gps
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
    //inicio de pistas para switch
    pinMode(buttonPin1, INPUT_PULLDOWN); //1 ronda
    pinMode(buttonPin2, INPUT_PULLDOWN); //2 ronda
    pinMode(buttonPin3, INPUT_PULLDOWN); //3 ronda
    pinMode(buttonPin4, INPUT_PULLDOWN); //subir datos

    // Inicializar comunicación serial para depuración
    Serial.begin(115200);   // ---------delete on production---------- 
    ss.begin(9600);

    // Inicializar la pantalla OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección 0x3C para la pantalla OLED
        display.println("Error OLED.       CODIGO BLOQUEADO");
        while (1); // Bloquea el código si no se inicializa (peligroso)
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println("- OLED Listo!\n");

    // Inicializar el MPU6050
    if (!mpu.begin()) {
        display.println("Error MPU6050.     CODIGO BLOQUEADO");
        while (1); // Bloquea el código si no se inicializa (peligroso)
    }
    display.println("- MPU6050 listo!");

    display.display();
    delay(3000);
   
 
}

int calcularEscalaDeMovimiento(float accelMagnitude) {
    int escala = 0;

    // Si la aceleración es cercana a la gravedad (reposo)
    if (fabs(accelMagnitude - 9.8) < 0.3 ) {
        return 0;  // Sin perturbación
    }

    // Aceleración brusca
    if (accelMagnitude > accelerationThreshold) {
        escala += map(accelMagnitude, accelerationThreshold, 15.0, 1, 5);  // Ajustar los límites según sea necesario
    }

    // Limita la escala a un máximo de 10
    if (escala > 10) {
        escala = 10;
    }

    return escala;
}

void leer_datos(int buttonPin, int ronda){
    
    while(digitalRead(buttonPin) == HIGH){

    // Leer aceleración y giroscopio.
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    // Leer datos del GPS
    while (ss.available() > 0)
    gps.encode(ss.read());

    float latitude = gps.location.lat();
    float longitude = gps.location.lng();

    // Calcula la magnitud del vector de aceleración
    float accelMagnitude = sqrt(sq(a.acceleration.x) + sq(a.acceleration.y) + sq(a.acceleration.z));

    // Determina la escala de perturbación
    int movimientoBrusco = calcularEscalaDeMovimiento(accelMagnitude);


    if(movimientoBrusco > 5){

        if(i !=20){
        globalArray[i] = "{";
        globalArray[i] += "\"lat\": " + String(latitude, 6) + ", ";
        globalArray[i] += "\"long\": " + String(longitude, 6) + ", ";
        globalArray[i] += "\"lvl\": " + String(movimientoBrusco) + ", ";
        globalArray[i] += "\"rnd\": \"" + String(ronda) + "\"";
        globalArray[i] += "}";
        }

    i++;
    }


    // Imprime datos
    display.setCursor(0,0);
    display.print("Perturbacion: ");
    display.print(movimientoBrusco);
    display.setCursor(0,10);
    display.print("latitud: ");
    display.println(latitude,6);
    display.setCursor(0,20);
    display.print("longitud: ");
    display.println(longitude,6);
    display.setCursor(0,30);
    display.print("Nro de satelites: ");
    display.println(gps.satellites.value());
    display.setCursor(0,40);
    display.print("Velocidad (km/h):");
    display.println(gps.speed.kmph());

    display.setCursor(0,50);
    display.print("puntos restan:");
    display.println(20-i);
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
    display.println("Iniciando Ronda 1 en ");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin1, 1);
}

void ronda2(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Iniciando Ronda 2 en");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin2, 2);
}

void ronda3(){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Iniciando Ronda 3 en");
    display.println(" 3");
    display.display();
    delay(1000);
    display.println(" 2");
    display.display();
    delay(1000);
    display.println(" 1");
    display.display();
    delay(1000);

    leer_datos(buttonPin3, 3);
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
    for (int x = 0; x < 10; x++)
    {
    display.print(">>");
    display.display();
    delay(1000);
    }

    for (int y = 0; y < 10; y++) {
    Serial.println(globalArray[y]);  // Imprime cada nombre en el monitor serial
    }
    i=0;

     display.print("listo, datos restablecidos");
    display.display();
    delay(1000);
    
    }else{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("-----En espera-----");
    display.setCursor(0, 10);
    display.print("-----iniciar?-----");
    display.setCursor(0, 20);
    display.print("switch 1/ronda 1/der"); 
     display.setCursor(0, 30);
    display.print("switch 2/ronda 2/cent"); 
     display.setCursor(0, 40);
    display.print("switch 3/ronda 3/izq"); 
    display.setCursor(0, 50);
    display.print("switch 4/subir data"); 
    display.display();
    delay(100);
    }
     

}



