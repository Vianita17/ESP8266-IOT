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

void setup(){

  Wire.begin();//Wire.begin(pinSDA, pinSCL);-con argumentos
  
  lcd.init();// initialize LCD                   
  lcd.backlight();// turn on LCD backlight
  lcd.createChar(0, c1);
  conectarWiFi();
  lcd.print("Conectando ...");

  
  configOTA(HOST_NAME, passwordOTA);

  ip=WiFi.localIP();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(WiFi.SSID());lcd.setCursor(lcdColumns-1,0);lcd.write(0);
  lcd.setCursor(0,1);
  lcd.print(ip.toString());lcd.setCursor(lcdColumns-1,1);lcd.write(0);
  delay(5000);

  
}

void loop(){
  loopOTA();
  
}
