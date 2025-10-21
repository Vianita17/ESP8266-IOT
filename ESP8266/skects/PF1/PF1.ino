#include <configWiFi.h>
#include <esp8266OTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define MSG_BUFFER_SIZE (50)

#define pinLamp D6

#define BAJA 29
#define ALTA 31

IPAddress ip;
int dTime = 200;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String HOST_NAME = "viana-";
String passwordOTA = "123456";
unsigned long int lastMsg = 0;
const int DHTPin = D5;
DHT dht(DHTPin, DHTTYPE);

void setup() {
  
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
  delay(2000);

  pinMode(pinLamp, OUTPUT);

  dht.begin();

  
}
void proceso(){
  //leer sensor
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float hi = dht.computeHeatIndex(false);
  lcd.clear();
  lcd.setCursor(0,0);lcd.print("Tempe: ");lcd.print(String(t) + (char)223 + "C");
  lcd.setCursor(0,1);lcd.print("Humidity: ");lcd.print(h);

  if(float(t) < BAJA){
    digitalWrite (pinLamp, LOW);
  } else if (float(t) >= ALTA){
    digitalWrite (pinLamp, HIGH);
  }
}

void loop() {
  loopOTA();
  unsigned long now = millis();
  if(now - lastMsg > 1000){
    proceso();
    lastMsg = now;
  }
  
}
