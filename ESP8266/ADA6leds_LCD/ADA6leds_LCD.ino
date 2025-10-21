#include <configWiFi.h>
#include <esp8266OTA.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Config. DHT11
#define DHTTYPE DHT11
#define DHTPin D3

#define BAJA 27
#define MEDIA 28
#define ALTA 29

DHT dht(DHTPin, DHTTYPE);

//config. LEDS
#define ON_OFF D5
#define BAJO D6
#define MEDIO D7
#define ALTO D8

// Config. FAN
int pinFAN = D4;

//config. LCD
IPAddress ip;
int dTime = 200;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String HOST_NAME = "viana-";
String passwordOTA = "123456";

long lastDisplay = 0;

void setup() {
  //Config. WIFI & LCD
  Wire.begin();
  
  lcd.init();            
  lcd.backlight();
  conectarWiFi();
  lcd.print("Conectando ...");

  
  configOTA(HOST_NAME, passwordOTA);

  ip=WiFi.localIP();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(WiFi.SSID());
  lcd.setCursor(0,1);
  lcd.print(ip.toString());
  dht.begin();

  // Configuración de los pines
  pinMode(ON_OFF, OUTPUT);
  pinMode(BAJO, OUTPUT);
  pinMode(MEDIO, OUTPUT);
  pinMode(ALTO, OUTPUT);

  digitalWrite(ON_OFF, LOW);
  digitalWrite(BAJO, LOW);
  digitalWrite(MEDIO, LOW);
  digitalWrite(ALTO, LOW);
  
  pinMode(pinFAN, OUTPUT);
  analogWrite(pinFAN, 0);  // Apagar el ventilador al inicio
  delay(4000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(String("Estado: ") + "OFF");
  lcd.setCursor(0, 1);
  lcd.print(String("Temp: ") + "NAN");

  
}

void proceso() {
  float temperatura = dht.readTemperature();

  if (isnan(temperatura)) {
    digitalWrite(ON_OFF, LOW);
    digitalWrite(BAJO, LOW);
    digitalWrite(MEDIO, LOW);
    digitalWrite(ALTO, LOW);
    analogWrite(pinFAN, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String("Estado: ") + "OFF");
    lcd.setCursor(0, 1);
    lcd.print(String("Temp: ") + "NAN");

  } 
  else {
    digitalWrite(ON_OFF, HIGH);
    if (temperatura <= BAJA) {
      digitalWrite(BAJO, HIGH);
      digitalWrite(MEDIO, LOW);
      digitalWrite(ALTO, LOW);
      analogWrite(pinFAN, 100);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String("Estado: ") + "ON ");
      lcd.setCursor(0, 1);
      lcd.print(String("Temp: ") + String(temperatura) + (char)223 + "C");
    } 
    else if (temperatura=MEDIA && temperatura < ALTA) {
      digitalWrite(BAJO, LOW);
      digitalWrite(MEDIO, HIGH);
      digitalWrite(ALTO, LOW);
      analogWrite(pinFAN, 312);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String("Estado: ") + "ON ");
      lcd.setCursor(0, 1);
      lcd.print(String("Temp: ") + String(temperatura) + (char)223 + "C");
    } 
    else if(temperatura >= ALTA){
      digitalWrite(BAJO, LOW);
      digitalWrite(MEDIO, LOW);
      digitalWrite(ALTO, HIGH);
      analogWrite(pinFAN, 900);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(String("Estado: ") + "ON ");
      lcd.setCursor(0, 1);
      lcd.print(String("Temp: ") + String(temperatura) + (char)223 + "C");
    }
  }
}

void loop() {
  loopOTA();
  
  unsigned long now = millis();
  if (now - lastDisplay > 1000) {
    lastDisplay = now;
    proceso();
  }
}
