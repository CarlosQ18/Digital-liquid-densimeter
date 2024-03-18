#include<LiquidCrystal.h> // Include the Liquid Crystal Display library 
#include <HX711.h> // Include the HX711 load cell amplifier library 


const int rs=2; // Connect the rs pin of the LCD to digital pin 2 of the Arduino
const int en=3; // Connect the enable pin of the LCD to digital pin 3 of the Arduino
const int d4=4; // Connect the d4 pin of the LCD to digital pin 4 of the Arduino
const int d5=5; // Connect the d5 pin of the LCD to digital pin 5 of the Arduino
const int d6=6; // Connect the d6 pin of the LCD to digital pin 6 of the Arduino
const int d7=7; // Connect the d7 pin of the LCD to digital pin 7 of the Arduino

const int LC_DT_PIN = 8; // Connect the HX711 board's DT pin to digital pin 8 of the Arduino
const int LC_SCK_PIN = 9; //  Connect the HX711 board's SCK pin to digital pin 9 of the Arduino
const int ZSP_BUTTON = 10;
const int TARA_BUTTON = 11;
const int RESET_BUTTON = 12;

boolean zsp = LOW;
boolean tara = LOW;
boolean rst = LOW;

bool Calibracion = true;

int Estado = 1;

float Gravity = 9.77; // Gravedad en Cali en m/s^2
float pi = 3.14159265358979323846;
float radio = 0.0095; //metros
float Altura = 0.152; //metros
float VolumenCilindro = pi*(radio)*(radio)* Altura; // metros cubicos
float PesoBarra = 124.61; // gramos

float Offset_Celda = 0.00;
float Fuerza_real = 0.00;
float tara_real = 0.00;

LiquidCrystal mydisplay(rs,en,d4,d5,d6,d7); // Create an object called "mydisplay"
HX711 load; // Create an object called "scale" 

void setup()
{
  Serial.begin(9600);
  pinMode(ZSP_BUTTON, INPUT);
  pinMode(TARA_BUTTON, INPUT);
  pinMode(RESET_BUTTON, INPUT);
  mydisplay.begin(16,2); // Initialize the LCD with 16 columns and 2 rows
  mydisplay.clear(); // Clear the contents displayed on the LCD, if any.
  load.begin(LC_DT_PIN, LC_SCK_PIN); // Initialize the HX711 amplifer 

}

void loop()
{
    if(Calibracion == true){
      if (Estado == 1){
        mydisplay.clear();
        mydisplay.setCursor(3,0);
        mydisplay.print("LEVANTE EL");
        mydisplay.setCursor(6,1);
        mydisplay.print("PESO");
        //Serial.println("LEVANTE EL PESO");
        zsp = digitalRead(ZSP_BUTTON);
        if (zsp == HIGH) {
          mydisplay.clear();
          mydisplay.setCursor(4,0);
          mydisplay.print("AJUSTANDO");
          mydisplay.setCursor(4,1);
          mydisplay.print("OFFSET...");
          //Serial.println("AJUSTANDO OFFSET");
          for (int i=0; i<20; i++) {
              Offset_Celda = Offset_Celda + (load.read()/20);
          }
          Estado++;
        }                 
      }
      else{
        mydisplay.clear();
        mydisplay.setCursor(3,0);
        mydisplay.print("SUELTE EL");
        mydisplay.setCursor(6,1);
        mydisplay.print("PESO");
        //Serial.println("SUELTE EL PESO");
        tara = digitalRead(TARA_BUTTON);
        if (tara == HIGH) {
          mydisplay.clear();
          mydisplay.setCursor(3,0);
          mydisplay.print("REALIZANDO");
          mydisplay.setCursor(5,1);
          mydisplay.print("TARA..");
          for (int i=0; i<20; i++) {
              tara_real = tara_real + ((load.read()-Offset_Celda)/20);
          }
          for (int i=0; i<20; i++) {
            Fuerza_real = Fuerza_real+((load.read() - Offset_Celda)*PesoBarra/tara_real*0.001*9.77)/20;  
          }
          Estado = 1;
          Calibracion = false;
        }
      }              
    }
    else{
      float Force = (load.read() - Offset_Celda)*PesoBarra/tara_real*0.001*Gravity-Fuerza_real; // Define a variable called "reading" to store the value from the amplifier 
      float density =abs(Force/(VolumenCilindro*Gravity));
      
      mydisplay.clear();
      mydisplay.setCursor(0,0); 
      mydisplay.print("DENSIDAD = ");
      
      
      mydisplay.setCursor(0,1); 
      mydisplay.print(density);
      //Serial.println(density);
      
      mydisplay.setCursor(9,1);
      mydisplay.print("Kg/m^3");
      mydisplay.print("      ");
    }
    
    rst = digitalRead(RESET_BUTTON);
    
    if (rst == HIGH){
      Calibracion = true;
      Estado = 1;
      Offset_Celda = 0.00;
      Fuerza_real = 0.00;
      tara_real = 0.00;
    }

   delay(1000);
}
