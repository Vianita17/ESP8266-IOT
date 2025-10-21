#include <WiFiManager.h>
#include <Ticker.h>

WiFiManager wifiManager;

#define pinLedWiFi D4 //D4 Puerto (GPIO2)
const char* HOSTPOT = "VIANA-ESP8266";
//Instancia a Ticker
Ticker ticker;

void parpadeoLedWiFi(){
  byte estado = digitalRead(pinLedWiFi);
  digitalWrite(pinLedWiFi, !estado);//si esta alto, va estar bajo, y viceversa
}

void conectarWiFi(){
  pinMode(pinLedWiFi, OUTPUT);//Modo salida
  ticker.attach(0.5, parpadeoLedWiFi);//El segundo parámetro es la función que se quiere ejecutar cada vez que pase el tiempo
  //wifiManager.resetSettings();
  if(!wifiManager.autoConnect(HOSTPOT)){
    ESP.reset();
    delay(1000);
  }
  ticker.detach();
  digitalWrite(pinLedWiFi, HIGH);
}
