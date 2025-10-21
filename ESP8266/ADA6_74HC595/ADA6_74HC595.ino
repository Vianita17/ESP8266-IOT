#include <configWiFi.h>
#include <esp8266OTA.h>
#include <DHT.h>

// Config. DHT11
#define DHTTYPE DHT11
#define DHTPin D3

#define BAJA 25
#define MEDIA 30
#define ALTA 35

DHT dht(DHTPin, DHTTYPE);

// Config. 74HC595
int latchPin = D7;
int clockPin = D8;
int dataPin = D6;
uint8_t ledState = 0b0000;  // Estado inicial de los LEDs

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
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  
  pinMode(pinFAN, OUTPUT);
  analogWrite(pinFAN, 0);  // Apagar el ventilador al inicio
  
  
  proceso();
}

void updateShiftRegister() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, ledState);
  digitalWrite(latchPin, HIGH);
}

void proceso() {
  float temperatura = dht.readTemperature();

  if (isnan(temperatura)) {
    ledState = 0b0000;
  } 
  else {
    if (temperatura <= BAJA) {
      analogWrite(pinFAN, 256);
      ledState = 0b0011;
    } 
    else if (temperatura > BAJA && temperatura < ALTA) {
      analogWrite(pinFAN, 512);
      ledState = 0b0101;
    } 
    else {
      analogWrite(pinFAN, 1023);
      ledState = 0b1001;
    }
  }
  
  updateShiftRegister();
}

void loop() {
  loopOTA();
  
  /*unsigned long now = millis();
  if (now - lastDisplay > 5000) {
    lastDisplay = now;
    proceso();
  }*/
}
