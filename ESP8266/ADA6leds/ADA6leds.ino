#include <configWiFi.h>
#include <esp8266OTA.h>
#include <DHT.h>

// Config. DHT11
#define DHTTYPE DHT11
#define DHTPin D3

#define BAJA 20
#define MEDIA 25
#define ALTA 28

DHT dht(DHTPin, DHTTYPE);

//config. LEDS
#define ON_OFF D5
#define BAJO D6
#define MEDIO D7
#define ALTO D8

// Config. FAN
int pinFAN = D4;

String HOST_NAME = "viana-";
String passwordOTA = "123456";

long lastDisplay = 0;

void setup() {
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  dht.begin();

  // Configuración de los pines
  pinMode(ON_OFF, OUTPUT);
  pinMode(BAJO, OUTPUT);
  pinMode(MEDIO, OUTPUT);
  pinMode(ALTO, OUTPUT);

  digitalWrite(ON_OFF, LOW);
  digitalWrite(BAJO, LOW);
  digitalWrite(MEDIO, LOW);
  digitalWrite(ALTO, LOW);
  
  pinMode(pinFAN, OUTPUT);
  analogWrite(pinFAN, 0);  // Apagar el ventilador al inicio
}

void proceso() {
  float temperatura = dht.readTemperature();

  if (isnan(temperatura)) {
    digitalWrite(ON_OFF, LOW);
    digitalWrite(BAJO, LOW);
    digitalWrite(MEDIO, LOW);
    digitalWrite(ALTO, LOW);
    analogWrite(pinFAN, 0);
  } 
  else {
    digitalWrite(ON_OFF, HIGH);
    if (temperatura <= BAJA) {
      digitalWrite(BAJO, HIGH);
      digitalWrite(MEDIO, LOW);
      digitalWrite(ALTO, LOW);
      analogWrite(pinFAN, 100);
    } 
    else if (temperatura > BAJA && temperatura < ALTA) {
      digitalWrite(BAJO, LOW);
      digitalWrite(MEDIO, HIGH);
      digitalWrite(ALTO, LOW);
      analogWrite(pinFAN, 512);
    } 
    else if(temperatura >= ALTA){
      digitalWrite(BAJO, LOW);
      digitalWrite(MEDIO, LOW);
      digitalWrite(ALTO, HIGH);
      analogWrite(pinFAN, 1023);
    }
  }
}

void loop() {
  loopOTA();
  
  unsigned long now = millis();
  if (now - lastDisplay > 5000) {
    lastDisplay = now;
    proceso();
  }
}
