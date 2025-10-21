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
const char* TOPIC_SUB  = "esp8266/murrieta/dht11/lab";
const char* TOPIC_PUB = "esp8266/viana/display"; // Unique topic for receiving commands

long lastDisplay = 0;
int contador = 0;

WiFiClient espClient;
PubSubClient client(espClient);

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

void setup() {
  Wire.begin(); // Wire.begin(pinSDA, pinSCL);-con argumentos
  
  lcd.init(); // initialize LCD                   
  lcd.backlight(); // turn on LCD backlight
  lcd.createChar(0, c1);
  lcd.print("Conectando ...");

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensaje = "";
  String mensaje2 = "";
  for (int i = 0; i < length; i++) {
    mensaje += (char)payload[i];
  }
  // Convert message to float (temperature in Celsius)
  float temperatureC = mensaje.toFloat();
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  // Display on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temperatureC) + (char)223 +"C");
  lcd.setCursor(0, 1);
  lcd.print("Temp: " + String(temperatureF) + " F");
}

/*String ConvertirF(float valor) {
  double conversion = (valor * 1.8) + 32; // Convert to Fahrenheit
  return String(conversion); // Return as String
}

void printLCD(const char* mensaje, const char* mensaje2) {
  lcd.setCursor(0, 0);
  lcd.print(mensaje);lcd.print(" °C");lcd.setCursor(lcdColumns-1,0);lcd.write(0);
  lcd.setCursor(0, 1);
  lcd.print(mensaje2);lcd.print(" °F");lcd.setCursor(lcdColumns-1,1);lcd.write(0);
  
}*/

void loop() {
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
      client.subscribe(TOPIC_SUB); // Subscribe to a unique topic
      client.publish(TOPIC_PUB, "Julian Viana conectado! XD");
    } else {
      delay(5000);
    }
  }
}
