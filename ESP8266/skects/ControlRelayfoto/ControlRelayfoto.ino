#include <configWiFi.h>
#include <esp8266OTA.h>

long lastDisplay = 0;


#define pinBUTTON D7
#define pinTOUCH D6
#define pinRELAY D5
#define pinFOTO A0

String HOST_NAME = "Viana-";
String passwordOTA = "123456";


int umbralLuz = 200; //0-1024

void setup() {

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  pinMode(pinBUTTON, INPUT);
  pinMode(pinTOUCH, INPUT);
  pinMode(pinRELAY, OUTPUT);
  pinMode(pinFOTO, INPUT);

  digitalWrite(pinRELAY, HIGH);
}

void loop() {
  loopOTA();
  unsigned long now = millis();
  if (now - lastDisplay > 100) {
    lastDisplay = now;
    proceso();
  }
}

void proceso() {
  int nivelLuz = analogRead(pinFOTO); 

  bool estadoBoton = digitalRead(pinBUTTON);
  bool estadoTouch = digitalRead(pinTOUCH);
  static bool prender = false;
  
 if (nivelLuz < umbralLuz) {
    digitalWrite(pinRELAY, LOW);
  } else {
    digitalWrite(pinRELAY, HIGH);
    if (estadoBoton == LOW){
      prender = !prender;
    }
    else{
    digitalWrite(pinRELAY, prender ? LOW : HIGH);
    }
  }
}
