#include <configWiFi.h>
#include <esp8266OTA.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Config. DHT11
#define DHTTYPE DHT11
#define DHTPin D3

#define BAJA 29
#define MEDIA 30
#define ALTA 31

DHT dht(DHTPin, DHTTYPE);

// Configuración 74HC595
#define DATA_PIN D5    // DS
#define CLOCK_PIN D6   // SH_CP
#define LATCH_PIN D7   // ST_CP

// Configuración de los bits para LEDs en el 74HC595
#define ON_OFF_BIT 0
#define BAJO_BIT 1
#define MEDIO_BIT 2
#define ALTO_BIT 3

// Configuración del ventilador
int pinFAN = D4;

// Configuración de la LCD
IPAddress ip;
int dTime = 200;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String HOST_NAME = "viana-";
String passwordOTA = "123456";

long lastDisplay = 0;

// Estado de los LEDs para el 74HC595
uint8_t ledState = 0;

// Función para enviar los datos al 74HC595
void updateShiftRegister() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, ledState);
  digitalWrite(LATCH_PIN, HIGH);
}

// Función para configurar un bit específico en ledState
void setLedState(uint8_t bit, bool state) {
  if (state) {
    ledState |= (1 << bit); // Encender el bit
  } else {
    ledState &= ~(1 << bit); // Apagar el bit
  }
  updateShiftRegister();
}

void setup() {
  // Configuración de WiFi y LCD
  Wire.begin();
  
  lcd.init();            
  lcd.backlight();
  conectarWiFi();
  lcd.print("Conectando ...");

  configOTA(HOST_NAME, passwordOTA);

  ip = WiFi.localIP();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.SSID());
  lcd.setCursor(0, 1);
  lcd.print(ip.toString());
  dht.begin();

  // Configuración de los pines del 74HC595
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  // Configuración del ventilador
  pinMode(pinFAN, OUTPUT);
  analogWrite(pinFAN, 0);  // Apagar el ventilador al inicio
  delay(4000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Estado: OFF");
  lcd.setCursor(0, 1);
  lcd.print("Temp: NAN");

  // Inicializar LEDs en OFF
  ledState = 0;
  updateShiftRegister();
}

void proceso() {
  float temperatura = dht.readTemperature();

  if (isnan(temperatura)) {
    // Apagar todos los LEDs y el ventilador si no hay lectura de temperatura
    setLedState(ON_OFF_BIT, LOW);
    setLedState(BAJO_BIT, LOW);
    setLedState(MEDIO_BIT, LOW);
    setLedState(ALTO_BIT, LOW);
    analogWrite(pinFAN, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: OFF");
    lcd.setCursor(0, 1);
    lcd.print("Temp: NAN");
  } 
  else if (temperatura < BAJA) {  // Rango de temperatura muy bajo, ventilador apagado
    setLedState(ON_OFF_BIT, LOW);
    setLedState(BAJO_BIT, LOW);
    setLedState(MEDIO_BIT, LOW);
    setLedState(ALTO_BIT, LOW);
    analogWrite(pinFAN, 0);  // Apagar el ventilador

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: OFF");
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(temperatura) + (char)223 + "C");
  }
  else if (temperatura >= BAJA && temperatura < MEDIA) {  // Rango BAJO
    setLedState(ON_OFF_BIT, HIGH);
    setLedState(BAJO_BIT, HIGH);
    setLedState(MEDIO_BIT, LOW);
    setLedState(ALTO_BIT, LOW);
    analogWrite(pinFAN, 60);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: ON ");
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(temperatura) + (char)223 + "C");
  }
  else if (temperatura >= MEDIA && temperatura < ALTA) {  // Rango MEDIO
    setLedState(ON_OFF_BIT, HIGH);
    setLedState(BAJO_BIT, LOW);
    setLedState(MEDIO_BIT, HIGH);
    setLedState(ALTO_BIT, LOW);
    analogWrite(pinFAN, 100);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: ON ");
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(temperatura) + (char)223 + "C");
  }
  else if (temperatura >= ALTA) {  // Rango ALTO
    setLedState(ON_OFF_BIT, HIGH);
    setLedState(BAJO_BIT, LOW);
    setLedState(MEDIO_BIT, LOW);
    setLedState(ALTO_BIT, HIGH);
    analogWrite(pinFAN, 255);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estado: ON ");
    lcd.setCursor(0, 1);
    lcd.print("Temp: " + String(temperatura) + (char)223 + "C");
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
