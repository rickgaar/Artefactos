#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

#define calibration_factor 716.45 //This value is obtained using the SparkFun_HX711_Calibration sketch

//********DELAY PARA MOSTRAR ESTADO DEL WIFI*********
unsigned long interval = 30000; // delay en ms
unsigned long previousMillis = 0;

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

//***************CONFIGURACIÓN DE RED****************
const char *ssid = "Internetinador";
const char *passwd = "PerryElOrnitorrinco";

//***************CONFIGURACIÓN DE MQTT***************
char *server = "192.168.252.13";
int port = 1883;

const char *mqtt_user = "rickgar";
const char *mqtt_password = "123456";

bool lowWater = false;

//time variables
unsigned long t;
unsigned long t2;
unsigned long daytime;
unsigned long readTime;
const unsigned long graceTime = 5000;

const unsigned long halfDay = 120000;
unsigned long halfHour = 2000;

//***************DEFINICIÓN DE VARIABLES GLOBALES******************

int IN1 = D0;
int IN2 = D1;
int IN3 = D2;
int IN4 = D3;
int ENB = D4;
float bascula = 0;
const float minimoBascula = 18.5;
const float maximoBascula = 20;
float consumedFood = 0;
float initialFood = 0;
float _capacitivo=0;
bool motorStarted = false;
bool derGiro = false;
bool pumpStarted = false;

const int DOUT=D6;
const int CLK=D5;

float difference = 0;

HX711 balanza;

// Definir todos los "topics" a los cuales se desea suscribir
const char* topics[] = {
  "/monitor/nivelAgua",
};

//***************CALLBACK DE RESOLUCIÓN A SUBSCRIBES***************
void mqttCallback(char *topicChar, byte *payload, unsigned int length) {
  Serial.println();

  //*********Obtener el topic convertido a tipo String*************
  String topic = String(topicChar);
  Serial.print("Message arrived on Topic: ");
  Serial.println(topic);

  //*********Obtener el mensaje convertido a tipo String***********
  String capacitivo = "";
  for (unsigned int i = 0; i < length; i++) {
    capacitivo += (char)payload[i];  // Añadir cada carácter al String
  }

  //**********Definir manejo de mensajes entrantes por topic*******
  //Ej:
  if (topic == "/monitor/nivelAgua") {
    Serial.println(capacitivo);
    _capacitivo=capacitivo.toFloat();
    if(capacitivo.toFloat() <= 15){
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      lowWater = true;
    }else{
      lowWater = false;
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }
  }
}

//***************FUNCIONES PARA PUBLICAR Y SUSCRIBIR***************
boolean publishToTopic(char *topic, char *message) {
  return mqttClient.publish(topic, message);
}

void subscribeToTopics() {
  const int numTopics = sizeof(topics) / sizeof(topics[0]);

   for (int i = 0; i < numTopics; i++) {
      mqttClient.subscribe(topics[i]);
  }
}

void setup() {
  //*********Sección para inicializar sensores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  //*********Inicializar monitor serial***********
  Serial.begin(115200);
  t = millis();
  daytime = millis();
  readTime = millis();

  //*********Configurar conexión wifi*************
  WiFi.begin(ssid, passwd);

  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.print("Connected to WiFi Access Point, Got an IP address: ");
  Serial.println(WiFi.localIP());
  // Configurar la reconexión automática ante perdida de señal
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);// Configurar la reconexión automática ante perdida de señal

  //*********Configurar conexión MQTT*************
  mqttClient.setServer(server, port);
  mqttClient.setCallback(mqttCallback);

  // Realizar conexión al MQTT Broker
  if (mqttClient.connect("ESP-Client", mqtt_user, mqtt_password)) {
    Serial.println("Connected to MQTT Broker");
  } else {
    Serial.println("MQTT Broker connection failed");
    Serial.println(mqttClient.state());
    delay(200);
  }

  // suscribirse a los temas.
  subscribeToTopics();

  Serial.begin(115200);
  balanza.begin(DOUT, CLK);
  balanza.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  balanza.tare(20);
}

void loop() {
  // Imprime la señal de la red cada <interval> segundos
  printWifiStatus();
  
  // verificar si no hay conexión con el broker, si es así reconectarse:
  if (!mqttClient.connected()) {
    reconnect(); 
  }

  // Loop de funcionamiento de la libreria PubSubClient
  mqttClient.loop();

  //********Tú código de sensores, funcionamiento, etc. Va aquí********

  //Se mide la balanza cada 30 minutos
    if(millis() - readTime >= halfHour){
      bascula = balanza.get_units(20);
      difference = initialFood - bascula;
      readTime = millis();

      if (difference > 0) {
        consumedFood += difference;
      }
      Serial.println(" Peso: ");
      Serial.print(bascula);
      Serial.print(" diferencia: ");
      Serial.print(difference);
      Serial.println();


      initialFood = bascula; // Actualizar el peso inicial
      difference = 0;
    }

    if(millis() > graceTime){
      //Si queda poca agua y si todavia es de dia
      if(pumpStarted || (lowWater && ((millis() - daytime) <= halfDay))){
        bombaAgua();
      }

      //Si queda poca comida y si todavia es de dia
      if(motorStarted || (bascula <= minimoBascula && ((millis() - daytime) <= halfDay))){
        motorreductor(bascula);
      }
    }
  
  //Cuando hallan pasado 24 horas, se reinicia el contador del dia
  //Al acaba el dia, se envia el total de comida consumida
  if(millis() - daytime >= halfDay*2){
    daytime = millis();
    char *floatString = floatToChar(consumedFood, 2); // El segundo parámetro es la cantidad de decimales
    publishToTopic("/monitor/comida", floatString);
    free(floatString); //Este paso es importantísimo para liberar la memoria reservada por el buffer, caso contrarió pueden haber problemas fuga, desbordamiento o fragmentación de memoria*/
  }
}

void reconnect() {
  while (!mqttClient.connected()) {
    Serial.println();
    Serial.println("Trying to connect to the MQTT broker...");

    if (mqttClient.connect("ESP-Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");

      // suscribirse nuevamente a los temas si la conexión regresa.
      subscribeToTopics();
    } else {
      Serial.print("Fallo, rc=");
      Serial.println(mqttClient.state());
      Serial.println("Trying to connect each 5 seconds");
      // Esperar 5 segundos antes de reintentar
      delay(5000);
    }
    printWifiStatus();
  }
}

void printWifiStatus() {
  // Imprime la señal de la red cada <interval> segundos
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    Serial.println("");
    Serial.println("----------------------------------------");
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("Wifi Configured SSID cannot be reached");
        break;
      case WL_CONNECTED:
        Serial.println("Connection Wifi successfully established");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Wifi Connection failed");
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    Serial.println("----------------------------------------");
    previousMillis = currentMillis;
  }
}

//***************FUNCIONES AUXILIARES DE CONVERSIÓN***************


//***************float -> *char********************
char* floatToChar(float number, int precision) {
    // Calcular el tamaño necesario (incluyendo el signo y el punto decimal)
    int length = snprintf(NULL, 0, "%.*f", precision, number);
    
    // Asignar memoria para la cadena resultante
    char* result = (char*)malloc(length + 1); // +1 para el terminador nulo
    
    // Verificar si la asignación fue exitosa
    if (result != NULL) {
        snprintf(result, length + 1, "%.*f", precision, number);
    }
    
    return result;
}

//***************int -> *char********************
char* intToChar(int number) {
    // Calcular el tamaño necesario (incluyendo el signo)
    int length = snprintf(NULL, 0, "%d", number);
    
    // Asignar memoria para la cadena resultante
    char* result = (char*)malloc(length + 1); // +1 para el terminador nulo
    
    // Verificar si la asignación fue exitosa
    if (result != NULL) {
        snprintf(result, length + 1, "%d", number);
    }
    
    return result;
}

void motorreductor(float bascula){
  if(!motorStarted){
    motorStarted = true;
    t = millis();
    halfHour = 100;
  }

  if(!derGiro && millis() - t <= 70.25){
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
    if(millis() - t <= 70.25){
      digitalWrite(IN4, HIGH);
      digitalWrite(IN3, LOW);
      analogWrite(ENB, 255);
    }else if(millis() - t <= 5053.25){
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      derGiro = false;
      motorStarted = false;
      halfHour = 2000;
    }
  }
}

void bombaAgua(){
  if(pumpStarted){
    pumpStarted = true;
    t2 = millis();
  }

  if(_capacitivo == 1024){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    Serial.print("Bomba, giro a la izquierda completado"); // Esta saca el agua
    lowWater = false;
    _capacitivo=0;
    pumpStarted=false;
  }
}