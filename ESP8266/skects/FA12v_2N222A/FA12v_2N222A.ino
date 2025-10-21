#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MSG_BUFFER_SIZE (50)
 
IPAddress ip;
int dTime = 200;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String HOST_NAME = "viana-";
String passwordOTA = "123456";

String texto = "Hello word";

byte c1[] = {
  B00000,
  B00100,
  B00110,
  B00100,
  B00100,
  B11111,
  B01110,
  B00000
};

int pinFAN = D3;
int long lastToggle = 0;


void setup(){
  
  pinMode(pinFAN, OUTPUT);
  digitalWrite(pinFAN, LOW);
  
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  
}

void loop(){
  loopOTA();

  int long now = millis();
  if(now - lastToggle > 10000){
    digitalWrite(pinFAN, !digitalRead(pinFAN));
    lastToggle = now;
    
  }
}
