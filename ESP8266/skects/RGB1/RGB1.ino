#include <configWiFi.h>
#include <esp8266OTA.h>

#define pinRed  D1 //led rojo
#define pinGreen D2 //led verde
#define pinBlue D3 //led blue
String HOST_NAME = "viana-";
String passwordOTA = "123456";

void color(int R, int G, int B){
  digitalWrite(pinRed, R);
  digitalWrite(pinGreen, G);
  digitalWrite(pinBlue, B);
}
void setup() {
  // put your setup code here, to run once:
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  digitalWrite(pinRed, 0);
  digitalWrite(pinGreen, 0);
  digitalWrite(pinBlue, 0);
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();
  color(1,0,0);//anodo(0,1,1)-catodo(1,0,0)
  delay(1000);
  color(0,1,0);//anodo(1,0,1)-catodo(0,1,0)
  delay(1000);
  color(0,0,1);//anodo(0,0,1)-catodo(0,1,0)
  delay(1000);
}
