#include <configWiFi.h>
#include <esp8266OTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include "html.h"
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

// MQTT
const char* URLServer = "esp8266murrieta.ddns.net"; // Dirección del servidor MQTT
const char* user_mqtt = "esp8266";
const char* user_pass = "iotesp8266";
String topicBase = "esp8266/";
String topicSubSet;
String topicXStatus;
String topicXSet;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg = 0;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// WebServer
ESP8266WebServer webServer(80);
bool loopServer = false;
String key_id = "";
String chipId;
IPAddress ip;

#define NUMBER_COMPONENTS 2
String components[NUMBER_COMPONENTS] = {"lamp", "numeric"}; // Componentes manejados por el dispositivo

int pinState;

#define TRIG_PIN D5  // Pin Trigger (sensor ultrasónico)
#define ECHO_PIN D6  // Pin Echo (sensor ultrasónico)
#define RELAY_PIN D7 // Pin para el relé
int deltaTime = 1000;
int lastPub = 0;

String HOSTNAME = "maldonado2";
String PASSWORDOTA = "123456";

unsigned long tiempoUltimaRevision = 0;  
unsigned long intervaloRevision = 10000; // Cada 10 segundos se revisa la distancia

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Función para medir la distancia con el sensor ultrasónico
long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH);
  long distancia = duracion * 0.034 / 2;
  // Retornar -1 si está fuera de rango
  return (distancia >= 2 && distancia <= 400) ? distancia : -1;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();

  chipId = String(ESP.getChipId());
  topicBase += chipId + "/";
  topicSubSet = topicBase + "set";
 
  conectarWiFi();
  ip = WiFi.localIP();

  String action = "register";
  String arguments = "&ip=" + ip.toString();

  ticker.attach(0.5, parpadeoLedWiFi);

  if (!deviceIsRegister(action, arguments, key_id)) {
    // Si no se registra el dispositivo, inicia el modo servidor local
    if (!MDNS.begin(HOSTNAME)) {
      Serial.println("Error al iniciar mDNS");
    } else {
      Serial.println("Servicio mDNS iniciado");
      initialize_wwwServer();
    }
  } else {
    // Si se registra, configura MQTT
    loopServer = false;
    ticker.detach();
    mqttClient.setServer(URLServer, 1883);
    mqttClient.setCallback(callback);
    configOTA(HOSTNAME, PASSWORDOTA);
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema iniciado");
  delay(2000);
  lcd.clear();

  Serial.println("Setup completado con éxito");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Mensaje recibido Topic:");
  Serial.print(topic);
  Serial.println("");
  
  if (strcmp(topic, topicSubSet.c_str()) == 0) {
    // Aquí antes se publicaba "ON" al recibir mensaje, 
    // pero ahora ya se publica "ON" al conectarse a MQTT.
    return;
  } else {
    topicXSet = topicBase + "0/set";
    if (strcmp(topic, topicXSet.c_str()) == 0) {
      topicXStatus = topicBase + "0/status";
      if (char(payload[0]) == '2') {
        pinState = digitalRead(RELAY_PIN);
      } else {
        digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));
        delay(10);
        pinState = digitalRead(RELAY_PIN);
      }

      if (pinState == LOW) {
         mqttClient.publish(topicXStatus.c_str(), "OFF");
      } else {
         mqttClient.publish(topicXStatus.c_str(), "ON");
      }
    }
  }
}
int publicarUltrasonico(long distancia) {
  if (distancia < 0) return -1;

  HTTPClient http;
  WiFiClient client;
  String url = "http://" + String(URLServer) + "/web/device/numeric?";
  url += "chip_id=" + chipId + "&value=" + String(distancia) + "&position=1";

  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode > 0 && httpCode == 200) {
    http.end();
    return 0;
  } else {
    http.end();
    return -1;
  }
}
void reconnect() {
  while (!mqttClient.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(), user_mqtt, user_pass)) {
      Serial.println("MQTT conectado!");
      // Suscribirse al tópico base
      mqttClient.subscribe(topicSubSet.c_str());

      // Publicar el estado online inmediatamente tras conectarse
      String topicPubStatus = topicBase + "status";
      mqttClient.publish(topicPubStatus.c_str(), "ON");
      Serial.println("Publicando estado en: " + topicPubStatus);

      // Suscribir a los tópicos de cada componente
      for (int j = 0; j < NUMBER_COMPONENTS; j++) {
        String topicSub = topicBase + String(j) + "/set";
        mqttClient.subscribe(topicSub.c_str());
        Serial.println("Subscrito a: " + topicSub);
      }
    } else {
      delay(5000);  
    }    
  } 
}
void loop() {
  loopOTA();
  
  if (loopServer) {
    // Si el dispositivo está en modo servidor web (no registrado aún), atiende las solicitudes del navegador
    webServer.handleClient();
  } else {
    // Si el dispositivo ya está registrado, entra en lógica MQTT
    if (!mqttClient.connected()) {
       reconnect();
    }
    mqttClient.loop();
  }

  // Cada intervalo definido se mide la distancia y se actúa en consecuencia
  if (millis() - tiempoUltimaRevision >= intervaloRevision) {
    tiempoUltimaRevision = millis();

    long distancia = medirDistancia();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distancia: ");
    lcd.print(distancia);
    lcd.print(" cm");

    if (distancia< 100) {
      lcd.setCursor(0, 1);
      lcd.print("Objeto cerca!");
      digitalWrite(RELAY_PIN, LOW); // Activar el relé si el objeto está cerca
    } else if (distancia < 0) {
      // La distancia negativa indica fuera de rango
      lcd.setCursor(0, 1);
      lcd.print("Fuera de rango");
      digitalWrite(RELAY_PIN, HIGH); // Desactivar el relé
    } else {
      // distancia >= 100 cm
      lcd.setCursor(0, 1);
      lcd.print("Sin objeto");
      digitalWrite(RELAY_PIN, HIGH); // Desactivar el relé
    }

    int result = publicarUltrasonico(distancia);
    if (result >= 0) {
      Serial.println("Datos del sensor enviados correctamente");
    } else {
      Serial.println("Error al enviar datos del sensor");
    }
  }
}


bool deviceIsRegister(String action, String arguments, String key_id) {
  HTTPClient http;
  WiFiClient client;
  String url = "http://" + String(URLServer) + "/web/device/";
  url += action + "?chip_id=" + chipId + "&setting_key_id=" + key_id;
  url += arguments;
  
  Serial.println(url);
  
  if (http.begin(client, url)) {
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();  

    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
        Serial.println(payload);
        if (payload.compareTo("registrado") == 0) {
          http.end();
          return true;
        } else if (payload.compareTo("insertado") == 0) {
          int conteo = 0;
          for (int j = 0; j < NUMBER_COMPONENTS; j++) {
            String action = "register-component";
            String arguments = "&type=" + components[j] + "&position=" + String(j);
            deviceIsRegister(action, arguments, key_id);
            conteo++;
            delay(5000);
          }
          
          String head = HTML_header;
          String foot = HTML_footer;
          String content = "El dispositivo con chip id " + chipId + " fue registrado con exito!!";
          content += " Contiene " + String(conteo) + " componente(s)";

          Serial.println("Enviando respuesta HTML...");
          webServer.send(200, "text/html", head + content + foot);
          
          delay(5000);
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
    Serial.printf("[HTTP] Unable to connect\n");
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

  String content = "<p>ESP8266 IoT: </p>";
  String formcontent = "A continuación, agrega tu Key_ID: <input type=\"text\" name=\"key_id\" value=\"" + key_id + "\">";

  webServer.send(200, "text/html", head + content + formhead + formcontent + formfoot + foot);
}

void handle_form() {
  String head = HTML_header;
  String foot = HTML_footer;
  String link = HTML_saved_link;

  if (webServer.hasArg("key_id") && webServer.arg("key_id") != NULL) {
    key_id = webServer.arg("key_id");
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
