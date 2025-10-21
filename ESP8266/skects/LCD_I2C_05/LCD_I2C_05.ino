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

  lcd.clear();
  lcd.home();
  lcd.print(texto);
  
}

void loop(){
  loopOTA();
  
  for (int positionCounter = 0; positionCounter < texto.length(); positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(dTime);
  }

  // scroll 29 positions (string length + display length) to the right
  // to move it offscreen right:
  for (int positionCounter = 0; positionCounter < texto.length() + lcdColumns ; positionCounter++) {
    // scroll one position right:
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(dTime);
  }

  // scroll 16 positions (display length) to the left
  // to move it back to center:
  for (int positionCounter = 0; positionCounter < lcdColumns; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(dTime);
  }

  // delay at the end of the full loop:
  delay(1000);
}
