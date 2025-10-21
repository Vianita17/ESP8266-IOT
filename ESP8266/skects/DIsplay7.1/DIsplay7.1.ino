#include <configWiFi.h>
#include <esp8266OTA.h>


String HOST_NAME = "viana-";
String passwordOTA = "123456";
int LEDs[] = {D0,D1,D2,D3,D4,D6,D7,D8}; // for ESP8266 microcontroller
const int deltaT = 1000;
int cero[]   = {0, 0, 0, 0, 0, 0, 1, 1}; // Estados de LEDs para mostrar el 0 (invertido)
int uno[]    = {1, 0, 0, 1, 1, 1, 1, 1}; // Estados de LEDs para mostrar el 1 (invertido)
int dos[]    = {0, 0, 1, 0, 0, 1, 0, 1}; // Estados de LEDs para mostrar el 2 (invertido)
int tres[]   = {0, 0, 0, 0, 1, 1, 0, 1}; // Estados de LEDs para mostrar el 3 (invertido)
int cuatro[] = {1, 0, 0, 1, 1, 0, 0, 1}; // Estados de LEDs para mostrar el 4 (invertido)
int cinco[]  = {0, 1, 0, 0, 1, 0, 0, 1}; // Estados de LEDs para mostrar el 5 (invertido)
int seis[]   = {0, 1, 0, 0, 0, 0, 0, 1}; // Estados de LEDs para mostrar el 6 (invertido)
int siete[]  = {0, 0, 0, 1, 1, 1, 1, 1}; // Estados de LEDs para mostrar el 7 (invertido)
int ocho[]   = {0, 0, 0, 0, 0, 0, 0, 1}; // Estados de LEDs para mostrar el 8 (invertido)
int nueve[]  = {0, 0, 0, 0, 1, 0, 0, 1}; // Estados de LEDs para mostrar el 9 (invertido)
int pd[]     = {1, 1, 1, 1, 1, 1, 1, 0}; // Estados de LEDs para mostrar el punto decimal (pd) (invertido)

long lastDisplay = 0;

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i<8; i++) pinMode(LEDs[i], OUTPUT);
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
}
void display(){
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], cero[i]);
  delay(deltaT);  
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], uno[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], dos[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], tres[i]);
  delay(deltaT);  
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], cuatro[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], cinco[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], seis[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], siete[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], ocho[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], nueve[i]);
  delay(deltaT);
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], pd[i]);
  delay(deltaT);    
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();
  long now = millis();
  if (now - lastDisplay > 15000){
    display();
    lastDisplay = now;
  }
}
