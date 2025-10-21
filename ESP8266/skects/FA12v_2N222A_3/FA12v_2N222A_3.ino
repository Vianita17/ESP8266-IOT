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
float porcentaje = 0.0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup(){
  pinMode(pinFAN, OUTPUT);
  analogWrite(pinFAN, LOW);
  
  lcd.init();            
  lcd.backlight();      
  lcd.clear();
  lcd.print("Conectando ...");
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  lcd.clear();
  lcd.print("Estado: ");lcd.print("OFF");
  lcd.setCursor(0, 1);
  lcd.print("Vld: ");lcd.print(porcentaje, 1);lcd.print("%   ");

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }

  int velocidad = mensaje.toInt(); 
  if (velocidad >= 0 && velocidad <= 1024) {
    analogWrite(pinFAN, velocidad);
    porcentaje = (velocidad / 1024.0)*100;
    lcd.setCursor(0, 0);
    lcd.print("Estado: ");
    if (velocidad > 0) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }

    lcd.setCursor(0, 1);
    lcd.print("Vld: ");
    lcd.print(porcentaje, 1);
    lcd.print("%   ");
  }
}

void loop(){
  loopOTA();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(TOPIC_SUB);
      client.publish("esp8266/status", "xd");
    } else {
      delay(5000);
    }
  }
}
