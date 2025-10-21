#include <WiFiManager.h>
#include <Ticker.h>
#include "ledRGB.h" 


WiFiManager wifiManager;

#define pinLedWiFi D4 //D4 Puerto (GPIO2)


const char* HOSTPOT = "VIANA-ESP8266";
//Instancia a Ticker
Ticker ticker;
Ticker tickerRGB;

void parpadeoLedWiFi(){
  byte estado = digitalRead(pinLedWiFi);
  digitalWrite(pinLedWiFi, !estado);//si esta alto, va estar bajo, y viceversa
}

void parpadeoLedWiFiRGB() {
  static bool estado2 = false; // Variable para alternar el estado
  if (estado2) {
    setColor(255, 128, 0); // Ámbar
  } else {
    setColor(0, 0, 0); // Apagar el LED
  }
  estado2 = !estado2; // Alternar el estado
}

void conectarWiFi(){
  pinMode(pinLedWiFi, OUTPUT);//Modo salida
  ticker.attach(0.5, parpadeoLedWiFi);//El segundo parámetro es la función que se quiere ejecutar cada vez que pase el tiempo
  tickerRGB.attach(0.5, parpadeoLedWiFiRGB);
  //wifiManager.resetSettings();
  
  if(!wifiManager.autoConnect(HOSTPOT)){
    ESP.reset();
    delay(1000);
  }
  ticker.detach();
  tickerRGB.detach();
  digitalWrite(pinLedWiFi, HIGH);
  setColor(0,255, 0); // Verde
}
