#include <configWiFiRGB.h>
#include <esp8266OTARGB.h>
#include "ledRGB.h" 

int pinRed = D1;
int pinGreen = D2;
int pinBlue = D3;

String HOST_NAME = "viana-";
String passwordOTA = "123456";

void setup() {
  // Configuración de pines para el LED RGB
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  
  // Iniciar indicador en rojo durante el setup
  setColor(255, 0, 0); // Rojo
  delay(2000); // Delay opcional para visualizar el color
  
  conectarWiFi();   // Conectar a WiFi
  configOTA(HOST_NAME, passwordOTA); // Configurar OTA
}

void loop() {
  loopOTA(); 
}
