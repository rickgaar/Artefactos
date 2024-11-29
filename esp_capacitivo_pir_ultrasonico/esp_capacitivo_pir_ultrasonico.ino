//******DEFINICIÓN DE VARIABLES GLOBALES*******

int Capacitivo = A0;
int TRIG = D1; 
int ECHO = D0;
int PIR = D2;
long t = 0; // Siempre que sea con tiempo, se usa long
float v = 0.000340; // velocidad en microsegundos
float d = 0;
float valorCapacitivo;

unsigned long daytime;
unsigned long readTime;
const unsigned long halfHour = 5000;
const unsigned long halfDay = 43200000;

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIR, INPUT);
  pinMode(Capacitivo, INPUT);

  Serial.begin(115200);
  daytime = millis();
  readTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - readTime >= halfHour){
      digitalWrite(TRIG, LOW); // Asegurarme que este apagado
      delayMicroseconds(2);
      // Encender trigger
      digitalWrite(TRIG, HIGH);
      delayMicroseconds(10); // Delay de microsegundos
      digitalWrite(TRIG, LOW);

      t = pulseIn(ECHO, HIGH); // Agarrar tiempo de ida y vuelta
      d = (v*t/2); // Distancia entre 2 por la ida y vuelta

      if(d >= 0.13){
        Serial.print("Rellene el tanque ");
      }else{
        Serial.print("El nivel ta bien ");
      } 

      readTime = millis();
      valorCapacitivo = analogRead(Capacitivo);
      Serial.print(" Valor capacitivo: ");
      Serial.print(valorCapacitivo);
      Serial.print(" Valor PIR: ");
      Serial.println(analogRead(PIR));
  }
}