#include "HX711.h"

int IN1 = D0;
int IN2 = D1;
int IN3 = D2;
int IN4 = D3;
int ENB = D4;
const int DOUT=D6;
const int CLK=D5;

float bascula = 0;
const float minimoBascula = 80;
const float maximoBascula = 120;

bool motorStarted = false;
bool izqGiro = false;
bool derGiro = false;

//time variables
unsigned long t;
unsigned long daytime;
unsigned long readTime;
unsigned long scaleTime;

const unsigned long halfDay = 43200000;
const unsigned long halfHour = 1800000;
const unsigned long grasaTime = 10000;

HX711 balanza;

void setup() {
  // put your setup code here, to run once:
   //*********Sección para inicializar sensores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  readTime = millis();
  scaleTime = millis();

  Serial.begin(115200);
  balanza.begin(DOUT, CLK);
  Serial.print("Lectura del valor del ADC:  ");
  Serial.println(balanza.read());
  Serial.println("No ponga ningun  objeto sobre la balanza");
  Serial.println("Destarando...");
  Serial.println("...");
  balanza.set_scale(570); // Establecemos la escala
  //balanza.tare(20);  //El peso actual es considerado Tara.
  
  Serial.println("Listo para pesar"); 
}

void loop() {
  // put your main code here, to run repeatedly:

  if(millis() - scaleTime >= 500){
    Serial.print("Peso: ");
    bascula = balanza.get_units(20);
    Serial.print(bascula,3);
    Serial.println(" grs");
    scaleTime = millis();
  }

  if(millis() - readTime >= grasaTime){
    //Si queda poca comida y si todavia es de dia
      if(motorStarted || (bascula <= minimoBascula && ((millis() - daytime) <= halfDay))){
        motorreductor(bascula);
      }
  }
}

void motorreductor(float bascula){
  if(!motorStarted){
    motorStarted = true;
    t = millis();
  }

  if(!derGiro && millis() - t <= 53.25){
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 255);
  }else if(!derGiro && millis() - t <= 5053.25){
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    derGiro=true;
    t=0;
  } 
  if (bascula>=maximoBascula){
    if(!t){
      t=millis();
    }
    if(millis() - t <= 53.25){
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
    analogWrite(ENB, 255);
  }else if(millis() - t <= 5053.25){
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    derGiro = false;
    motorStarted = false;
  }
  }
}
