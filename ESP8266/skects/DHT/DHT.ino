#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT11

const int DHTPin = D5;
int pinLed = D2;

const char* MOTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana/dht11";
const char* TOPIC_SUB = "esp8266/viana/led";

String HOST_NAME = "viana-";
String passwordOTA = "123456";

unsigned long int lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

WiFiClient espClient;
PubSubClient client(espClient);//cliente

//crear instancia a la libreria
DHT dht(DHTPin, DHTTYPE);
void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  client.setServer(MOTT_SERVER, 1883);
  client.setCallback(callback);
  
  dht.begin();

  
}

//CHECAR PAR VER SI ESTA CONECTADO AL SERVIDOR
void reconnect(){
  while(!client.connected()){
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      client.subscribe(TOPIC_SUB);
      client.publish(TOPIC_PUB, "Julian Viana conectado! ");
    }else{
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length){
  if((char)payload[0] == '1'){
    digitalWrite(pinLed, HIGH);
  }else if((char)payload[0] == '0'){
    digitalWrite(pinLed, LOW);
  }
}
void enviarMensaje(){
  digitalWrite(LED_BUILTIN, LOW);
  ++value;
  //leer sensor
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float hi = dht.computeHeatIndex(false);
  snprintf(msg, MSG_BUFFER_SIZE, "T: %5.2f°C, H: %5.2f%%, HI: %5.2f°C", t, h ,hi);
  client.publish(TOPIC_PUB, msg);
  digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();
  
  if(!client.connected()){
    reconnect();
  }

  unsigned long now = millis();
  if(now - lastMsg > 30000){
    enviarMensaje();
    lastMsg = now;
  }
  
  client.loop();
}
