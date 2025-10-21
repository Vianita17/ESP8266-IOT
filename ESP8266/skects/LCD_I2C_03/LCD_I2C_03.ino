#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define MSG_BUFFER_SIZE (50)

IPAddress ip;

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String HOST_NAME = "viana-";
String passwordOTA = "123456";


void setup(){

  Wire.begin();//Wire.begin(pinSDA, pinSCL);-con argumentos
  
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  lcd.init();// initialize LCD                   
  lcd.backlight();// turn on LCD backlight
  lcd.print("Conectando ...");
  

  ip=WiFi.localIP();
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print(WiFi.SSID());
  lcd.setCursor(0,1);
  lcd.print(ip.toString());
  delay(5000);

}

void loop(){
  loopOTA();

}
