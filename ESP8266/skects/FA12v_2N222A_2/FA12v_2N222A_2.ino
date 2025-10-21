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

const char* MQTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_SUB  = "esp8266/fan";

int pinFAN = D3;
int long lastToggle = 0;

void setup(){
  pinMode(pinFAN, OUTPUT);
  digitalWrite(pinFAN, LOW);
  
  lcd.init();            
  lcd.backlight();      
  lcd.clear();
  lcd.print("Conectando ...");
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  lcd.clear();
  lcd.print("Fan Status:");

}

void loop(){
  loopOTA();

  int long now = millis();
  if(now - lastToggle > 10000){
    digitalWrite(pinFAN, !digitalRead(pinFAN));
    lastToggle = now;

    lcd.setCursor(0, 1);
    if (digitalRead(pinFAN) == HIGH) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }
  }
}
