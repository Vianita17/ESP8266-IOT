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
  
  lcd.init();// initialize LCD                   
  lcd.backlight();// turn on LCD backlight
  lcd.print("Conectando ...");

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  ip=WiFi.localIP();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(WiFi.SSID());
  lcd.setCursor(0,1);
  lcd.print(ip.toString());
  delay(5000);

  int contador = 0;
  int fila = 0;
  int col = 0;
  int dTime = 2000;
  lcd.clear();
  lcd.setCursor(col, fila);

  for(int i = 32; i <= 255; i++){
    lcd.print(i); lcd.print(" ");lcd.print((char)i);lcd.print(" ");
    contador++;
    if(contador > 1){
      fila++;
      contador=0;
      lcd.setCursor(col,fila);
      delay(dTime);
    }
    if(fila > lcdRows - 1){
      fila = 0;
      contador = 0;
      lcd.clear();
      lcd.home();
    }
  }
  lcd.clear();
  lcd.home();
  lcd.print("OTA!");  
}

void loop(){
  loopOTA(); 

}
