#include <esp8266OTA.h>
#include <configWiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include "html.h"
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



//MQTT
const char* URLServer = "esp8266murrieta.ddns.net"; //web y mqtt
const char* user_mqtt = "esp8266";
const char* user_pass = "iotesp8266";
String topicBase = "esp8266/";
String topicSubSet;
String topicXStatus;
String topicXSet;
#define MSG_BUFFER_SIZE (5)
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg  = 0;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
 

//WebServer
ESP8266WebServer webServer(80);
bool loopServer = false;
String key_id = "";
String chipId;
IPAddress ip;

//OTA
String HOSTNAME = "viana";
String PASSWORDOTA = "123456";

//lcd
int dTime = 200;
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 


//sever componentes
#define NUMBER_COMPONENTS 3
String components[NUMBER_COMPONENTS] = {"lamp", "dht", "switch"};
int pinState;

//Start components
#define pinLamp D6
#define pinSwitch D7
#define pinDht D5

#define BAJA 26
#define ALTA 28

int deltaTime = 1000;
int lastPub = 0;
DHT dht(pinDht, DHT11);

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Wire.begin();//Wire.begin(pinSDA, pinSCL);-con argumentos
  lcd.init();// initialize LCD                   
  lcd.backlight();// turn on LCD backlight
  lcd.print("Configurando ...");
  chipId = String(ESP.getChipId());
  topicBase += chipId + "/";
  topicSubSet = topicBase + "set";
  Serial.println("Topic sub set " + topicSubSet);
  conectarWiFi();
  ip = WiFi.localIP(); 

  String action =  "register";
  String arguments = "&ip=" + ip.toString();  
  ticker.attach(0.5, parpadeoLedWiFi);
  if(!deviceIsRegister(action, arguments, key_id)) {
     if (!MDNS.begin(HOSTNAME)) {
        Serial.println("Error al iniciar mDNS");
     } else {
      Serial.println("Servicio mDNS iniciado");
      initialize_wwwServer();
     }
  } else {
     loopServer = false;
     ticker.detach();
     mqttClient.setServer(URLServer, 1883);
     mqttClient.setCallback(callback);
     configOTA(HOSTNAME, PASSWORDOTA);
  }
  pinMode(pinLamp, OUTPUT);
  pinMode(pinSwitch, OUTPUT);
  pinMode(pinDht, INPUT);
  digitalWrite(pinLamp, HIGH);
  dht.begin();
  lcd.setCursor(0,0);lcd.print("Sin registro");
}

 

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Mensaje recibido Topic:");
  Serial.print(topic);
  Serial.println("");

  // Leer la temperatura actual para las condiciones
  float t = dht.readTemperature();  // Suponiendo que tienes acceso al DHT

  if (strcmp(topic, topicSubSet.c_str()) == 0) {
    String topicPubStatus = topicBase + "status";
    mqttClient.publish(topicPubStatus.c_str(), "ON");
    Serial.println("Publique :" + topicPubStatus);
    return;
  } else {
    // Condición: solo permitir acciones según la temperatura
    if (t < BAJA) {
      Serial.println("Temperatura menor a BAJA, deshabilitado.");
      return;  // No hacer nada si la temperatura es menor a BAJA
    }

    if (t >= BAJA && t < ALTA) {
      Serial.println("Temperatura entre MEDIA y ALTA, permite interacción.");
      topicXSet = topicBase + "0/set";
      if (strcmp(topic, topicXSet.c_str()) == 0) {
        topicXStatus = topicBase + "0/status";
        if (char(payload[0]) == '2') {
          pinState = digitalRead(pinLamp);
        } else {
          digitalWrite(pinLamp, !digitalRead(pinLamp));
          delay(10);
          pinState = digitalRead(pinLamp);
        }
      }   
      topicXSet = topicBase + "2/set";
      if (strcmp(topic, topicXSet.c_str()) == 0) {
        topicXStatus = topicBase + "2/status";
        if (char(payload[0]) == '2') {
          pinState = digitalRead(pinSwitch);
        } else {
          digitalWrite(pinSwitch, !digitalRead(pinSwitch));
          delay(10);
          pinState = digitalRead(pinSwitch);
        }
      }
    }

    if (t >= ALTA) {
      Serial.println("Temperatura mayor o igual a ALTA, interacción deshabilitada.");
      return;  // No hacer nada si la temperatura es mayor o igual a ALTA
    }
  }

  // Publicar el estado del pin
  if (pinState == LOW) {
    mqttClient.publish(topicXStatus.c_str(), "OFF");
  } else {
    mqttClient.publish(topicXStatus.c_str(), "ON");
  }
}


void reconnect() {
  while(!mqttClient.connected()){
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(mqttClient.connect(clientId.c_str(), user_mqtt, user_pass)) {
      Serial.println("MQTT conectado!");
      mqttClient.subscribe(topicSubSet.c_str());
      mqttClient.publish(topicSubSet.c_str(), "ON");

      //Los componentes
      for(int j=0; j<=NUMBER_COMPONENTS; j++){
        String topicSub = topicBase + String(j) + "/set";
        mqttClient.subscribe(topicSub.c_str());
        Serial.println("Subscrito a: " + topicSub);
      }
    } else {
      delay(100);  
    }    
  } 
}

 

int publicarDHT() {
  HTTPClient http;
  WiFiClient client;
  String url = "http://" + String(URLServer) + "/web/device/dht?";
  url += "chip_id=" + chipId + "&humidity=";
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float hi = dht.computeHeatIndex(false);
  if(!isnan(h) || !isnan(t)) {
     url += String(h);
     url += "&temperature=" + String(t);
     url += "&heatindex=" + String(hi);
     //logica nueva
     
     lcd.clear();
     lcd.setCursor(0,0);lcd.print("Tempe: ");lcd.print(String(t) + (char)223 + "C");
     lcd.setCursor(0,1);lcd.print("Humidity: ");lcd.print(String(h));
     if(float(t) < BAJA){
        digitalWrite (pinLamp, LOW);
      } 
      else if(t >= BAJA && t < ALTA){
        digitalWrite (pinLamp, LOW);
      }
      else if (float(t) >= ALTA){
        digitalWrite (pinLamp, HIGH);
      }

      //logica incrustada
  } else {
    Serial.println("Error al leer el sensor DHT");
  }
  Serial.println(url);
  if (http.begin(client, url)) {
    //Iniciar conexión
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();  // Realizar petición
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();  // Obtener respuesta
        Serial.println(payload);  // Mostrar respuesta por serial
        http.end();
        return payload.toInt();
      }
    }
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }
  return deltaTime;
}

void loop() {
  loopOTA();
  if(loopServer) {
    webServer.handleClient();
  } else {
    //Publicar DHT
    if(millis() - lastPub > deltaTime) {
       lastPub = millis();
       deltaTime = publicarDHT();
    }
    //MQTT
    if(!mqttClient.connected()){
       reconnect();
    }
    mqttClient.loop();
  }
}

 

bool deviceIsRegister(String action, String arguments, String key_id) {
  HTTPClient http;
  WiFiClient client;
  String url = "http://" + String(URLServer) + "/web/device/";
  url += action + "?chip_id=" + chipId + "&setting_key_id=" + key_id;
  url += arguments;
  Serial.println(url);
  if (http.begin(client, url)) //Iniciar conexión
  {
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();  // Realizar petición
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();  // Obtener respuesta
        Serial.println(payload);  // Mostrar respuesta por serial
        if (payload.compareTo("registrado") == 0) {
          http.end();
          return true;
        } else if (payload.compareTo("insertado") == 0) {
          int conteo = 0;
          //Registrar cada componente que controla el dispositivo
          for(int j = 0; j<NUMBER_COMPONENTS; j++){
            String action = "register-component";
            String arguments = "&type=" + components[j] + "&position=" + String(j);
            deviceIsRegister(action, arguments, key_id);
            conteo ++;
            delay(100);
          }     
          String head = HTML_header;
          String foot = HTML_footer;
          String content = "<h2> El dispositivo con chip id " + chipId + "fue registrado con exito!! </h2>";
          content += "<p> Contiene " + String(conteo) + " componente(s) </p>";
          webServer.send(200, "text/html", head + content + foot);
          delay(100);
          restartESP();
        }
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
  return false;
}

void restartESP() {
  ESP.restart();  
}
void initialize_wwwServer() {
  webServer.on("/", handle_root);
  webServer.on("/save", HTTP_POST, handle_form);
  webServer.onNotFound(handle_not_found);
  webServer.begin();
  Serial.println("Web Server iniciado");
  loopServer = true; 
}
void handle_root() {
  String head = HTML_header;
  String foot = HTML_footer;
  String formhead = HTML_form_header;
  String formfoot = HTML_form_footer;
  String content = "<h2>ESP8266 IoT</h2>";
  String formcontent = "Enter key_id: <input type=\"text\" name=\"key_id\" value=\"" + key_id + "\">";  
  webServer.send(200, "text/html", head + content + formhead + formcontent + formfoot + foot);
}
void handle_form() {
  String head = HTML_header;
  String foot = HTML_footer;
  String link = HTML_saved_link;
  if ( webServer.hasArg( "key_id" ) && webServer.arg( "key_id" ) != NULL ) {
    key_id = webServer.arg( "key_id" );
    key_id.trim();
    String action = "register";
    String arguments = "&ip=" + ip.toString();
    deviceIsRegister(action, arguments, key_id);
  }
  webServer.send(200, "text/html", head + link + foot);
}
void handle_not_found() {
  webServer.send(404, "text/plain", "Page not found");
}
